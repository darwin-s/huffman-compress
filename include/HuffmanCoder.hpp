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
#include <istream>
#include <ostream>
#include <unordered_map>
#include <string>

namespace hfm {

class HuffmanCoder {
public:
    typedef std::unordered_map<unsigned char, std::string> Dictionary;
    typedef std::unordered_map<std::string, unsigned char> ReverseDictionary;

public:
    void generateDictionary(std::istream& input);
    const Dictionary& getDictionary() const;
    void loadDictionary(const Dictionary& dictionary);
    void compress(std::istream& input, std::ostream& output);
    void decompress(std::istream& input, std::ostream& output, unsigned long originalSize);

private:
    void fillFrequencies(const unsigned char* data, unsigned long dataSize, int* frequencies);
    HuffmanNode* generateTree(const int* frequencies);
    void fillDictionary(const HuffmanNode* root, std::string code = "");
    HuffmanNode* generateTreeFromDictionary();

private:
    Dictionary m_dictionary;
    ReverseDictionary m_reverseDictionary;
};

}

#endif //!HFM_HUFFMANCODER_HPP