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

#ifndef HFM_HUFFMANDECODER_HPP
#define HFM_HUFFMANDECODER_HPP

#include <PriorityQueue.hpp>
#include <unordered_map>
#include <string>

namespace hfm {

class HuffmanDecoder {
public:
    typedef std::unordered_map<unsigned char, std::string> Dictionary;
    typedef std::unordered_map<std::string, unsigned char> ReverseDictionary;

public:
    HuffmanDecoder(const char* inBuff, unsigned long buffSize);
    HuffmanDecoder(const HuffmanDecoder& other) = delete; // Non-copyable
    HuffmanDecoder(HuffmanDecoder&& other) noexcept;
    ~HuffmanDecoder() = default;
    void loadDictionary(const Dictionary& dict);
    ReverseDictionary& getDecodingDictionary();
    long decompress(char* outBuff, unsigned long numBytes);
    std::uint64_t getLastBytes() const;

    HuffmanDecoder&
        operator=(const HuffmanDecoder& other) = delete; // Non-copyable
    HuffmanDecoder& operator=(HuffmanDecoder&& other) noexcept;

private:
    HuffmanNode* generateTreeFromDictionary();
    void loadDictionaryFromStream();

private:
    ReverseDictionary m_dict;
    const char* m_inBuff;
    unsigned long m_inBuffSize;
    bool m_dictLoaded;
    unsigned long m_originalSize;

    // Compression state
    std::uint64_t m_processed; // Number of processed bytes
    std::uint64_t m_acc;       // 64-bit Accumulator for codes
    unsigned int m_accUsed;    // Used bits in the accumulator
    HuffmanNode* m_tree;       // Huffman tree
    std::uint64_t m_read;      // Number of bytes read from buffer
    std::uint64_t m_lastBytes; // Number of bytes processed last time
};

}

#endif //! HFM_HUFFMANDECODER_HPP