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
#include <cstdint>

namespace {

constexpr int FREQ_SIZE = 256;
constexpr int BUFF_SIZE = 1024 * 8;
constexpr int BYTES = 8;
constexpr int BITS = 64;

}

namespace hfm {

void HuffmanCoder::generateDictionary(std::istream& input) {
    unsigned char* data = nullptr;
    unsigned long dataSize = 0;
    int frequencies[FREQ_SIZE];

    input.seekg(0, std::ios::end);
    dataSize = input.tellg();
    input.seekg(0, std::ios::beg);

    data = new unsigned char[dataSize];
    input.read(reinterpret_cast<char*>(data), dataSize);

    fillFrequencies(data, dataSize, frequencies);
    HuffmanNode* root = generateTree(frequencies);
    fillDictionary(root);

    delete[] data;
}

const HuffmanCoder::Dictionary& HuffmanCoder::getDictionary() const {
    return m_dictionary;
}

void HuffmanCoder::loadDictionary(const Dictionary& dictionary) {
    for (const std::pair<unsigned char, std::string> it : dictionary) {
        m_reverseDictionary[it.second] = it.first; // Flip dictionary when loading
    }
}

void HuffmanCoder::compress(std::istream& input, std::ostream& output) {
    unsigned char* data = nullptr;

    // Get input data size
    input.seekg(0, std::ios::end);
    unsigned long dataSize = input.tellg();
    input.seekg(0, std::ios::beg);

    std::uint64_t part = 0; // 64 bit acuumulator
    unsigned int bitsWrote = 0; // Number of bits written to the accumulator
    unsigned int bytesWrote = 0; // Number of bytes written to the buffer
    std::uint8_t buffer[BUFF_SIZE]; // Output buffer
    int bitsLeft = 0; // Number of bits left to process in current code

    // Read all input data
    data = new unsigned char[dataSize];
    input.read(reinterpret_cast<char*>(data), dataSize);

    // Do this for every byte of the data
    for (unsigned long i = 0; i < dataSize; i++) {
        // Get corresponding code for current byte
        std::string code = m_dictionary[data[i]];
        bitsLeft = code.size(); // Get the size of the code

        // Do this untill the full code has been processed
        while (bitsLeft > 0) {
            // if accumulator is full
            if (bitsWrote == BITS) {
                // Write 64 bit integer to byte array
                for (int i = 0; i < BYTES; i++) {
                    buffer[bytesWrote + i] = (part >> ((BYTES - i - 1) * BYTES)) & 0xFF;
                }
                // Reset counters
                bitsWrote = 0;
                part = 0;
                bytesWrote += BYTES; // Increase number of bytes written into the array

                // If array is full, then flush it to the output stream and reset the counter
                if (bytesWrote == BUFF_SIZE) {
                    output.write(reinterpret_cast<const char*>(buffer), BUFF_SIZE);
                    bytesWrote = 0;
                }
            } else {
                // If current bit is 0 then add a 0 the the acuumulator (shift left)
                // otherwise shift left and add a 1
                if (code[code.size() - bitsLeft] == '0') {
                    part <<= 1;
                } else {
                    part <<= 1;
                    part |= 1;
                }

                bitsLeft--;
                bitsWrote++;
            }
        }
    }

    // If there are bits that were not written to the buffer
    if (bitsWrote > 0) {
        // Pad them with 0 at the end
        for (int i = 0; i < BITS - bitsWrote; i++) {
            part <<= 1;
        }

        // Write them to the buffer
        for (int i = 0; i < BYTES; i++) {
            buffer[bytesWrote + i] = (part >> ((BYTES - i - 1) * BYTES)) & 0xFF;
        }

        bytesWrote += BYTES;
    }

    // If there are bytes, that were not written to the output stream
    // then flush them
    if (bytesWrote > 0) {
        output.write(reinterpret_cast<const char*>(buffer), bytesWrote);
    }
}

void HuffmanCoder::decompress(std::istream& input, std::ostream& output, unsigned long originalSize) {
    // Generate a huffman tree from the current dictionary
    HuffmanNode* root = generateTreeFromDictionary();
    HuffmanNode* r = root;
    unsigned char* data = nullptr;

    // Get input data size
    input.seekg(0, std::ios::end);
    unsigned long dataSize = input.tellg();
    input.seekg(0, std::ios::beg);

    std::uint64_t part = 0; // 64 bit value with accumulated codes
    unsigned int bitsRead = 0; // Number of bits read from the accumulator
    unsigned int bytesWrote = 0; // Bytes wrote to the output buffer
    unsigned int bytesRead = BYTES; // Number of bytes read from the input buffer
    unsigned char buffer[BUFF_SIZE / 8]; // Output buffer

    // Read all input data
    data = new unsigned char[dataSize];
    input.read(reinterpret_cast<char*>(data), dataSize);

    // Read a 64 bit integer from the input byte array
    for (int i = 0; i < BYTES; i++) {
        part |= (static_cast<std::uint64_t>(data[i]) << ((BYTES - i - 1) * BYTES));
    }

    // The next part gets every bit from the input buffer
    // and traverses the generated huffman tree

    // Original size == number of codes, so this runs for every code
    for (unsigned long i = 0; i < originalSize; i++) {
        std::string code = "";
        // Process untill we have a full code
        while (true) {
            // If we read the whole accumulator, then read a new
            // 64 bit value from the buffer and reset the counter
            if (bitsRead == BITS) {
                for (int i = 0; i < BYTES; i++) {
                    part |= (static_cast<std::uint64_t>(data[bytesRead + i]) << ((BYTES - i - i) * BYTES));
                }   
                bytesRead += BYTES;
                bitsRead = 0;
            }

            // Get the MSB bit
            int bit = (part >> (BITS - 1)) & 1;

            // If current node is a leaf, then we have a full code
            // so add the corresponding character to the output buffer
            if (r->left == nullptr && r->right == nullptr) {
                // If output buffer is full, then flush it to the output stream
                // and reset the counter
                if (bytesWrote == BUFF_SIZE / 8) {
                    output.write(reinterpret_cast<const char*>(buffer), BUFF_SIZE / 8);
                    bytesWrote = 0;
                }

                buffer[bytesWrote] = m_reverseDictionary[code];
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
            part <<= 1;
            bitsRead++;
        }

        // Start over for every code
        r = root;
    }

    // If there is pending output, then flush it to the output stream
    if (bytesWrote > 0) {
        output.write(reinterpret_cast<const char*>(buffer), bytesWrote);
    }

    delete[] data;
}

void HuffmanCoder::fillFrequencies(const unsigned char* data, const unsigned long dataSize, int* frequencies) {
    for (int i = 0; i < FREQ_SIZE; i++) {
        frequencies[i] = 0;
    }

    for (unsigned long i = 0; i < dataSize; i++) {
        frequencies[static_cast<unsigned int>(data[i])]++;
    }
}

HuffmanNode* HuffmanCoder::generateTree(const int* frequencies) {
    PriorityQueue pq;

    for (int i = 0; i < FREQ_SIZE; i++) {
        if (frequencies[i] != 0) {
            HuffmanNode* n = new HuffmanNode;
            n->left = nullptr;
            n->right = nullptr;
            n->frequency = frequencies[i];
            n->symbol = static_cast<unsigned char>(i);

            pq.insert(n);
        }
    }

    while (pq.getSize() != 1) {
        HuffmanNode* left = pq.popMin();
        HuffmanNode* right = pq.popMin();
        HuffmanNode* top = new HuffmanNode;

        top->frequency = left->frequency + right->frequency;
        top->symbol = HuffmanNode::NO_SYMBOL;
        top->left = left;
        top->right = right;

        pq.insert(top);
    }

    return pq.popMin();
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

HuffmanNode* HuffmanCoder::generateTreeFromDictionary() {
    HuffmanNode* root = new HuffmanNode;
    HuffmanNode* r = root;
    root->frequency = 0;
    root->symbol = HuffmanNode::NO_SYMBOL;
    root->left = nullptr;
    root->right = nullptr;

    for (const std::pair<std::string, unsigned char> it : m_reverseDictionary) {
        std::string code = it.first;
        while (!code.empty()) {
            if (code[0] == '0') {
                if (r->left == nullptr) {
                    r->left = new HuffmanNode;
                    r->left->frequency = 0;
                    r->left->symbol = HuffmanNode::NO_SYMBOL;
                    r->left->left = nullptr;
                    r->left->right = nullptr;
                }

                r = r->left;
            } else {
                if (r->right == nullptr) {
                    r->right = new HuffmanNode;
                    r->right->frequency = 0;
                    r->right->symbol = HuffmanNode::NO_SYMBOL;
                    r->right->left = nullptr;
                    r->right->right = nullptr;
                }

                r = r->right;
            }

            code.erase(0, 1);
        }

        r->symbol = it.second;
        r = root;
    }

    return root;
}

}