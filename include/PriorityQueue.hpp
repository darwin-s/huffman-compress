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

#include <vector>

namespace hfm {

class PriorityQueue {
public:
    struct TreeNode {
        TreeNode();
        ~TreeNode();

        static constexpr unsigned char NO_SYMBOL = '\0'; // Means that there is no symbol
        int frequency; // Frequency of a certain symbol
        unsigned char symbol; // The actual symbol of the node
        TreeNode* left;
        TreeNode* right;
    };

public:
    PriorityQueue();
    ~PriorityQueue();
    void insert(const TreeNode& node);
    TreeNode getMin() const;
    TreeNode popMin();
    int getSize() const;

private:
    int getParentIndex(int nodeIndex) const;
    int getLeftIndex(int nodeIndex) const;
    int getRightIndex(int nodeIndex) const;
    void siftUp(int index);
    void siftDown(int index);

private:
    std::vector<TreeNode*> m_queue;
};

}

#endif //!HFM_PRIORITYQUEUE_HPP