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

#ifndef HFM_HUFFMANCODER_HPP
#define HFM_HUFFMANCODER_HPP

#include <PriorityQueue.hpp>
#include <unordered_map>
#include <string>
#include <cstdint>

namespace hfm {

class HuffmanCoder {
public:
    typedef std::unordered_map<unsigned char, std::string> Dictionary;

public:
    HuffmanCoder(char* inBuff, unsigned long buffSize);
    HuffmanCoder(const HuffmanCoder& other) = delete; // Non-copyable
    HuffmanCoder(HuffmanCoder&& other) noexcept;
    ~HuffmanCoder() = default;
    Dictionary& getDictionary();
    void loadDictionary(const Dictionary& dictionary);
    long compress(char* outBuff, unsigned long numBytes);

    HuffmanCoder& operator=(const HuffmanCoder& other) = delete; // Non-copyable
    HuffmanCoder& operator=(HuffmanCoder&& other) noexcept;

private:
    void generateDictionary();
    void fillFrequencies(int* frequencies);
    void generateTree(const int* frequencies);
    void fillDictionary(const HuffmanNode* root, std::string code = "");
    unsigned int writeStreamHeader(char* outBuff);

private:
    Dictionary m_dictionary;
    PriorityQueue m_tree;
    char* m_inBuff;
    char* m_inEnd;
    unsigned long m_buffSize;
    bool m_headerWritten;

    // Compression state
    std::uint64_t m_acc;    // 64-bit Accumulator for codes
    unsigned int m_accUsed; // Used bits in the accumulator
};

}

#endif //! HFM_HUFFMANCODER_HPP