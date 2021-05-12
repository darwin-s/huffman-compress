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

#ifndef HFM_HUFFMANNODE_HPP
#define HFM_HUFFMANNODE_HPP

namespace hfm {

class HuffmanNode {
public:
    HuffmanNode();
    HuffmanNode(const HuffmanNode& other);
    HuffmanNode(HuffmanNode&& other) noexcept;
    ~HuffmanNode();

    HuffmanNode& operator=(const HuffmanNode& other);
    HuffmanNode& operator=(HuffmanNode&& other) noexcept;

public:
    static constexpr unsigned char NO_SYMBOL = '\0'; // Means that there is no symbol associated
    int frequency; // Frequency of a certain symbol
    unsigned char symbol; // The actual symbol of the node
    HuffmanNode* left;
    HuffmanNode* right;
};

}

#endif //!HFM_HUFFMANNODE_HPP