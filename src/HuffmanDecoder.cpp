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

#include <HuffmanDecoder.hpp>

namespace {

constexpr int BYTES = 8;
constexpr int BITS  = 64;

}

namespace hfm {

HuffmanDecoder::HuffmanDecoder(const char* inBuff, unsigned long buffSize)
    : m_inBuff(inBuff), m_inBuffSize(buffSize), m_dictLoaded(false),
      m_originalSize(0), m_processed(0), m_acc(0), m_accUsed(0),
      m_tree(nullptr), m_read(0), m_lastBytes(0) {}

HuffmanDecoder::HuffmanDecoder(HuffmanDecoder&& other) noexcept
    : m_dict(std::move(other.m_dict)), m_inBuff(other.m_inBuff),
      m_inBuffSize(other.m_inBuffSize), m_dictLoaded(other.m_dictLoaded),
      m_originalSize(other.m_originalSize), m_processed(other.m_processed),
      m_acc(other.m_acc), m_accUsed(other.m_accUsed), m_tree(other.m_tree),
      m_read(other.m_read), m_lastBytes(other.m_lastBytes) {
    other.m_inBuff       = nullptr;
    other.m_inBuffSize   = 0;
    other.m_dictLoaded   = false;
    other.m_originalSize = 0;
    other.m_processed    = 0;
    other.m_acc          = 0;
    other.m_accUsed      = 0;
    other.m_tree         = nullptr;
    other.m_read         = 0;
    other.m_lastBytes    = 0;
}

void HuffmanDecoder::loadDictionary(const Dictionary& dict) {
    for (const auto& it : dict) {
        m_dict[it.second] = it.first;
    }
}

HuffmanDecoder::ReverseDictionary& HuffmanDecoder::getDecodingDictionary() {
    return m_dict;
}

long HuffmanDecoder::decompress(char* outBuff, unsigned long numBytes) {
    if (m_dict.empty()) {
        loadDictionaryFromStream();
        // Read a 64 bit integer from the input byte array
        for (int i = 0; i < BYTES; i++) {
            std::uint8_t byte     = m_inBuff[i];
            std::uint8_t shiftNo  = (BYTES - i - 1) * BYTES;
            std::uint64_t shifted = static_cast<std::uint64_t>(byte) << shiftNo;
            m_acc |= shifted;
        }
        m_read = BYTES;
    }

    // if we reached the end of the input
    if (m_processed >= m_originalSize) {
        return -1; // Signal end of buffer
    }

    // Generate a huffman tree from the current dictionary
    if (m_tree == nullptr) {
        m_tree = generateTreeFromDictionary();
    }
    HuffmanNode* r = m_tree;

    unsigned int bytesWrote = 0; // Number of bytes written to the buffer
    m_lastBytes             = 0;

    // The next part gets every bit from the input buffer
    // and traverses the generated huffman tree
    for (unsigned long i = 0; i < numBytes; i++) {
        // if we reached the end of the input
        if (m_processed >= m_originalSize) {
            return -2; // Signal end of buffer and flush needed
        }

        std::string code = "";
        // Process untill we have a full code
        while (true) {
            // If we read the whole accumulator, then read a new
            // 64 bit value from the buffer and reset the counter
            if (m_accUsed == BITS) {
                for (int i = 0; i < BYTES; i++) {
                    std::uint8_t byte     = m_inBuff[m_read + i];
                    std::uint8_t shiftNo  = (BYTES - i - 1) * BYTES;
                    std::uint64_t shifted = static_cast<std::uint64_t>(byte)
                                            << shiftNo;
                    m_acc |= shifted;
                }
                m_read += BYTES;
                m_accUsed = 0;
            }

            // Get the MSB bit
            int bit = (m_acc >> (BITS - 1)) & 1;

            // If current node is a leaf, then we have a full code
            // so add the corresponding character to the output buffer
            if (r->left == nullptr && r->right == nullptr) {
                outBuff[bytesWrote] = m_dict[code];
                m_processed++;
                m_lastBytes++;
                bytesWrote++;
                break;
            }

            // If msb is 0, then traverse to the left subtree,
            // otherwise go to the right
            if (bit == 0) {
                r = r->left;
                code += '0';
            } else {
                r = r->right;
                code += '1';
            }

            // Shift the accumulator to the left, so the next bit becomes MSB
            m_acc <<= 1;
            m_accUsed++;
        }

        // Start over for every code
        r = m_tree;
    }

    return bytesWrote;
}

std::uint64_t HuffmanDecoder::getLastBytes() const {
    return m_lastBytes;
}

HuffmanDecoder& HuffmanDecoder::operator=(HuffmanDecoder&& other) noexcept {
    m_dict         = std::move(other.m_dict);
    m_inBuff       = other.m_inBuff;
    m_inBuffSize   = other.m_inBuffSize;
    m_dictLoaded   = other.m_dictLoaded;
    m_originalSize = other.m_originalSize;
    m_processed    = other.m_processed;
    m_acc          = other.m_acc;
    m_accUsed      = other.m_accUsed;
    m_tree         = other.m_tree;
    m_read         = other.m_read;
    m_lastBytes    = other.m_lastBytes;

    other.m_inBuff       = nullptr;
    other.m_inBuffSize   = 0;
    other.m_dictLoaded   = false;
    other.m_originalSize = 0;
    other.m_processed    = 0;
    other.m_acc          = 0;
    other.m_accUsed      = 0;
    other.m_tree         = nullptr;
    other.m_read         = 0;
    other.m_lastBytes    = 0;

    return *this;
}

HuffmanNode* HuffmanDecoder::generateTreeFromDictionary() {
    HuffmanNode* root = new HuffmanNode;
    HuffmanNode* r    = root;
    root->frequency   = 0;
    root->symbol      = HuffmanNode::NO_SYMBOL;
    root->left        = nullptr;
    root->right       = nullptr;

    for (const auto& it : m_dict) {
        std::string code = it.first;
        while (!code.empty()) {
            if (code[0] == '0') {
                if (r->left == nullptr) {
                    r->left            = new HuffmanNode;
                    r->left->frequency = 0;
                    r->left->symbol    = HuffmanNode::NO_SYMBOL;
                    r->left->left      = nullptr;
                    r->left->right     = nullptr;
                }

                r = r->left;
            } else {
                if (r->right == nullptr) {
                    r->right            = new HuffmanNode;
                    r->right->frequency = 0;
                    r->right->symbol    = HuffmanNode::NO_SYMBOL;
                    r->right->left      = nullptr;
                    r->right->right     = nullptr;
                }

                r = r->right;
            }

            code.erase(0, 1);
        }

        r->symbol = it.second;
        r         = root;
    }

    return root;
}

void HuffmanDecoder::loadDictionaryFromStream() {
    // Read original size
    m_originalSize = reinterpret_cast<const unsigned long*>(m_inBuff)[0];
    m_inBuff += sizeof(unsigned long);
    // Read dictionary szie
    std::uint8_t dictSize = reinterpret_cast<const std::uint8_t*>(m_inBuff)[0];
    m_inBuff += sizeof(std::uint8_t);
    // Read dictionary
    for (unsigned i = 0; i < dictSize; i++) {
        // Read code size
        std::uint8_t codeSize =
            reinterpret_cast<const std::uint8_t*>(m_inBuff)[0];
        m_inBuff += sizeof(std::uint8_t);

        // Read code
        std::string code;
        for (unsigned j = 0; j < codeSize; j++) {
            code += reinterpret_cast<const char*>(m_inBuff)[0];
            m_inBuff++;
        }

        // Read byte and assign to dictionary
        m_dict[code] = reinterpret_cast<const unsigned char*>(m_inBuff)[0];
        m_inBuff++;
    }
}

}