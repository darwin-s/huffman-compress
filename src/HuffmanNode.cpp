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

#include <HuffmanNode.hpp>

namespace {

void copyChildren(hfm::HuffmanNode* dest, const hfm::HuffmanNode* src) {
    if (src->left != nullptr) {
        dest->left = new hfm::HuffmanNode;
        dest->left->frequency = src->left->frequency;
        dest->left->symbol = src->left->symbol;
        dest->left->left = nullptr;
        dest->left->right = nullptr;
        copyChildren(dest->left, src->left);
    }

    if (src->right != nullptr) {
        dest->right = new hfm::HuffmanNode;
        dest->right->frequency = src->right->frequency;
        dest->right->symbol = src->right->symbol;
        dest->right->left = nullptr;
        dest->right->right = nullptr;
        copyChildren(dest->right, src->right);
    }
}

}

namespace hfm {

HuffmanNode::HuffmanNode() 
    : frequency(0), symbol(NO_SYMBOL), left(nullptr), right(nullptr) {
}

HuffmanNode::HuffmanNode(const HuffmanNode& other) 
    : frequency(other.frequency), symbol(other.symbol) {
    left = nullptr;
    right = nullptr;
    copyChildren(this, &other);
}

HuffmanNode::HuffmanNode(HuffmanNode&& other) noexcept
    : frequency(other.frequency), symbol(other.symbol), left(other.left), right(other.right) {
    other.left = nulltpr;
    other.right = nullptr;
}

HuffmanNode::~HuffmanNode() {
    if (left != nullptr) {
        delete left;
    }

    if (right != nullptr) {
        delete right;
    }
}

HuffmanNode& HuffmanNode::operator=(const HuffmanNode& other) {
    frequency = other.frequency;
    symbol = other.symbol;
    left = nullptr;
    right = nullptr;
    copyChildren(this, &other);

    return *this;
}

HuffmanNode& HuffmanNode::operator=(HuffmanNode&& other) noexcept {
    frequency = other.frequency;
    symbol = other.symbol;
    left = other.left;
    right = other.right;

    other.left = nulltpr;
    other.right = nullptr;

    return *this;
}

}