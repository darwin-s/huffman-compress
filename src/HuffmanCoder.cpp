// Copyright 2021 Sirbu Dan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <HuffmanCoder.hpp>
#include <stdexcept>

namespace {

constexpr int FREQ_SIZE = 256;
constexpr int BYTES     = 8;
constexpr int BITS      = 64;

}

namespace hfm {

HuffmanCoder::HuffmanCoder(char* inBuff, unsigned long buffSize)
    : m_inBuff(inBuff), m_inEnd(inBuff + buffSize), m_buffSize(buffSize),
      m_headerWritten(false), m_acc(0), m_accUsed(0) {}

HuffmanCoder::HuffmanCoder(HuffmanCoder&& other) noexcept
    : m_dictionary(std::move(other.m_dictionary)), m_inBuff(other.m_inBuff),
      m_inEnd(other.m_inEnd), m_buffSize(other.m_buffSize),
      m_headerWritten(other.m_headerWritten), m_acc(other.m_acc),
      m_accUsed(other.m_accUsed) {
    other.m_inBuff        = nullptr;
    other.m_inEnd         = nullptr;
    other.m_buffSize      = 0;
    other.m_headerWritten = false;
    other.m_acc           = 0;
    other.m_accUsed       = 0;
}

HuffmanCoder::Dictionary& HuffmanCoder::getDictionary() {
    return m_dictionary;
}

void HuffmanCoder::loadDictionary(const Dictionary& dictionary) {
    m_dictionary = dictionary;
}

long HuffmanCoder::compress(char* outBuff, unsigned long numBytes) {
    if (m_dictionary.empty()) {
        generateDictionary();
        if (m_dictionary.empty()) {
            throw std::runtime_error("Unable to create dictionary");
        }
    }

    // if we reached the end of the input
    if (m_inBuff == m_inEnd) {
        // If there are bits that were not written to the buffer
        // then flush them
        if (m_accUsed > 0) {
            // Pad them with 0 at the end
            for (int i = 0; i < BITS - m_accUsed; i++) {
                m_acc <<= 1;
            }

            // Write them to the buffer
            for (int i = 0; i < BYTES; i++) {
                outBuff[i] = (m_acc >> ((BYTES - i - 1) * BYTES)) & 0xFF;
            }

            return -2; // Signal flush needed
        }

        return -1; // Signal end of buffer
    }

    // Write header if it was not written before
    unsigned int prefixSize = 0; // Size of prefixed bytes
    if (!m_headerWritten) {
        prefixSize      = writeStreamHeader(outBuff);
        m_headerWritten = true;
    }

    unsigned int bytesWrote =
        prefixSize;   // Number of bytes written to the buffer
    int bitsLeft = 0; // Number of bits left to process in current code

    // Do this for every byte of the data
    for (unsigned long i = 0; i < numBytes; i++) {
        // Check if reached end of input
        if (&m_inBuff[i] == m_inEnd) {
            m_inBuff = m_inEnd;
            break;
        }

        // Get corresponding code for current byte
        std::string code = m_dictionary[m_inBuff[i]];
        bitsLeft         = code.size(); // Get the size of the code

        // Do this untill the full code has been processed
        while (bitsLeft > 0) {
            // if accumulator is full
            if (m_accUsed == BITS) {
                // Write 64 bit integer to byte array
                for (int i = 0; i < BYTES; i++) {
                    outBuff[bytesWrote + i] =
                        (m_acc >> ((BYTES - i - 1) * BYTES)) & 0xFF;
                }
                // Reset counters
                m_accUsed = 0;
                m_acc     = 0;
                bytesWrote += BYTES;
            } else {
                // If current bit is 0 then add a 0 the the acuumulator (shift
                // left) otherwise shift left and add a 1
                if (code[code.size() - bitsLeft] == '0') {
                    m_acc <<= 1;
                } else {
                    m_acc <<= 1;
                    m_acc |= 1;
                }

                bitsLeft--;
                m_accUsed++;
            }
        }
    }

    // Update input buffer if end was not reached
    if (m_inBuff != m_inEnd) {
        m_inBuff += numBytes;
    }

    return bytesWrote;
}

HuffmanCoder& HuffmanCoder::operator=(HuffmanCoder&& other) noexcept {
    // Move fields
    m_dictionary    = std::move(other.m_dictionary);
    m_inBuff        = other.m_inBuff;
    m_inEnd         = other.m_inEnd;
    m_buffSize      = other.m_buffSize;
    m_headerWritten = other.m_headerWritten;
    m_acc           = other.m_acc;
    m_accUsed       = other.m_accUsed;

    // Invalidate fields of other
    other.m_inBuff        = nullptr;
    other.m_inEnd         = nullptr;
    other.m_buffSize      = 0;
    other.m_headerWritten = false;
    other.m_acc           = 0;
    other.m_accUsed       = 0;

    return *this;
}

void HuffmanCoder::generateDictionary() {
    int frequencies[FREQ_SIZE];

    fillFrequencies(frequencies);
    generateTree(frequencies);
    fillDictionary(m_tree.getMin());
}

void HuffmanCoder::fillFrequencies(int* frequencies) {
    for (int i = 0; i < FREQ_SIZE; i++) {
        frequencies[i] = 0;
    }

    for (unsigned long i = 0; i < m_buffSize; i++) {
        frequencies[reinterpret_cast<unsigned char*>(m_inBuff)[i]]++;
    }
}

void HuffmanCoder::generateTree(const int* frequencies) {
    for (int i = 0; i < FREQ_SIZE; i++) {
        if (frequencies[i] != 0) {
            HuffmanNode* n = new HuffmanNode;
            n->left        = nullptr;
            n->right       = nullptr;
            n->frequency   = frequencies[i];
            n->symbol      = static_cast<unsigned char>(i);

            m_tree.insert(n);
        }
    }

    while (m_tree.getSize() != 1) {
        HuffmanNode* left  = m_tree.popMin();
        HuffmanNode* right = m_tree.popMin();
        HuffmanNode* top   = new HuffmanNode;

        top->frequency = left->frequency + right->frequency;
        top->symbol    = HuffmanNode::NO_SYMBOL;
        top->left      = left;
        top->right     = right;

        m_tree.insert(top);
    }
}

void HuffmanCoder::fillDictionary(const HuffmanNode* root, std::string code) {
    if (root->left != nullptr) {
        code += '0';
        fillDictionary(root->left, code);
        code.pop_back();
    }

    if (root->right != nullptr) {
        code += '1';
        fillDictionary(root->right, code);
        code.pop_back();
    }

    if (root->right == nullptr && root->left == nullptr) {
        m_dictionary[root->symbol] = code;
    }
}

unsigned int HuffmanCoder::writeStreamHeader(char* outBuff) {
    unsigned int written = 0;
    // Write original data size
    reinterpret_cast<unsigned long*>(outBuff)[0] = m_buffSize;
    written += sizeof(m_buffSize);
    outBuff += written;
    // Write dictionary size
    reinterpret_cast<std::uint8_t*>(outBuff)[0] = m_dictionary.size();
    written += sizeof(std::uint8_t);
    outBuff += sizeof(std::uint8_t);
    // Write dictionary
    for (const auto& c : m_dictionary) {
        // Write code size
        reinterpret_cast<std::uint8_t*>(outBuff)[0] = c.second.size();
        written += sizeof(std::uint8_t);
        outBuff += sizeof(std::uint8_t);
        // Write code
        for (const auto& chr : c.second) {
            outBuff[0] = chr;
            written++;
            outBuff++;
        }
        // Write byte for code
        reinterpret_cast<unsigned char*>(outBuff)[0] = c.first;
        written++;
        outBuff++;
    }

    return written;
}

}