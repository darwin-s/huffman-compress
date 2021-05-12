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

#include <PriorityQueue.hpp>
#include <cmath>

namespace {

constexpr int INITIAL_SIZE = 1024;

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

PriorityQueue::PriorityQueue() {
    m_queue.reserve(INITIAL_SIZE);
}

PriorityQueue::PriorityQueue(const PriorityQueue& other) {
    m_queue.reserve(other.m_queue.size());
    for (const auto* n : other.m_queue) {
        HuffmanNode* newNode = new HuffmanNode;
        newNode->frequency = n->frequency;
        newNode->symbol = n->symbol;
        newNode->left = nullptr;
        newNode->right = nullptr;
        copyChildren(newNode, n);
    }
}

PriorityQueue::PriorityQueue(PriorityQueue&& other) noexcept {
    m_queue.reserve(other.m_queue.size());
    for (auto* n : other.m_queue) {
        m_queue.push_back(n);
    }
    other.m_queue.clear();
}

PriorityQueue::~PriorityQueue() {
    for (auto* n : m_queue) {
        delete n;
    }
}

void PriorityQueue::insert(const HuffmanNode& node) {
    HuffmanNode* n = new HuffmanNode;
    n->frequency = node.frequency;
    n->symbol = node.symbol;
    n->left = node.left;
    n->right = node.right;

    m_queue.push_back(n);

    // "Heapify" upwards
    siftUp(m_queue.size() - 1);
}

HuffmanNode PriorityQueue::getMin() const {
    HuffmanNode* min = m_queue[0];
    HuffmanNode ret;

    ret.frequency = min->frequency;
    ret.symbol = min->symbol;
    ret.left = min->left;
    ret.right = min->right;

    return ret;
}

HuffmanNode PriorityQueue::popMin() {
    HuffmanNode min = getMin();
    HuffmanNode* last = m_queue[m_queue.size() - 1];

    // Remove last element and put it at the beginning
    m_queue.pop_back();
    if (!m_queue.empty()) {
        m_queue[0] = last;
        // Now "heapify downwards"
        siftDown(0);
    }

    return min;
}

int PriorityQueue::getSize() const {
    return m_queue.size();
}

PriorityQueue& PriorityQueue::operator=(const PriorityQueue& other) {
    m_queue.reserve(other.m_queue.size());
    for (const auto* n : other.m_queue) {
        HuffmanNode* newNode = new HuffmanNode;
        newNode->frequency = n->frequency;
        newNode->symbol = n->symbol;
        newNode->left = nullptr;
        newNode->right = nullptr;
        copyChildren(newNode, n);
    }

    return *this;
}

PriorityQueue& PriorityQueue::operator=(PriorityQueue&& other) noexcept {
    m_queue.reserve(other.m_queue.size());
    for (auto* n : other.m_queue) {
        m_queue.push_back(n);
    }
    other.m_queue.clear();

    return *this;
}

int PriorityQueue::getParentIndex(const int nodeIndex) const {
    return static_cast<int>(std::floor((static_cast<double>(nodeIndex) - 1.0) / 2.0));
}

int PriorityQueue::getLeftIndex(const int nodeIndex) const {
    return 2 * nodeIndex + 1;
}

int PriorityQueue::getRightIndex(const int nodeIndex) const {
    return 2 * nodeIndex + 2;
}

void PriorityQueue::siftUp(int index) {
    int parentIndex = getParentIndex(index);

    while (parentIndex >= 0 && m_queue[parentIndex]->frequency > m_queue[index]->frequency) {
        HuffmanNode* tmp = m_queue[parentIndex];
        m_queue[parentIndex] = m_queue[index];
        m_queue[index] = tmp;

        index = parentIndex;
        parentIndex = getParentIndex(index);
    }
}

void PriorityQueue::siftDown(const int index) {
    int leftIndex = getLeftIndex(index);
    int rightIndex = getRightIndex(index);
    int smallestIndex = index;

    if (leftIndex < m_queue.size() && m_queue[leftIndex]->frequency < m_queue[smallestIndex]->frequency) {
        smallestIndex = leftIndex;
    }

    if (rightIndex < m_queue.size() && m_queue[rightIndex]->frequency < m_queue[smallestIndex]->frequency) {
        smallestIndex = rightIndex;
    }

    if (smallestIndex != index) {
        HuffmanNode* tmp = m_queue[smallestIndex];
        m_queue[smallestIndex] = m_queue[index];
        m_queue[index] = tmp;

        siftDown(smallestIndex);
    }
}

}
