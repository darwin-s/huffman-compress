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

#ifndef HFM_PRIORITYQUEUE_HPP
#define HFM_PRIORITYQUEUE_HPP

#include <HuffmanNode.hpp>
#include <vector>

namespace hfm {

class PriorityQueue {
public:
    PriorityQueue();
    PriorityQueue(const PriorityQueue& other);
    PriorityQueue(PriorityQueue&& other) noexcept;
    ~PriorityQueue();
    void insert(const HuffmanNode& node);
    HuffmanNode getMin() const;
    HuffmanNode popMin();
    int getSize() const;

    PriorityQueue& operator=(const PriorityQueue& other);
    PriorityQueue& operator=(PriorityQueue&& other) noexcept;

private:
    int getParentIndex(int nodeIndex) const;
    int getLeftIndex(int nodeIndex) const;
    int getRightIndex(int nodeIndex) const;
    void siftUp(int index);
    void siftDown(int index);

private:
    std::vector<HuffmanNode*> m_queue;
};

}

#endif //!HFM_PRIORITYQUEUE_HPP