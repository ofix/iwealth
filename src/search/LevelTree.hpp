#ifndef LevelTee_H
#define LevelTee_H

#include <queue>
#include <vector>
#include "util/Global.h"

/**
 * @param N 非叶子节点类型
 * @param L 叶子节点类型指针
 */
template <typename N, typename L>
class LevelTreeNode {
   public:
    // 成员函数
    LevelTreeNode(N non_leaf_node, LevelTreeNode<N, L>* parent = nullptr);
    LevelTreeNode(L* leaf_node, LevelTreeNode<N, L>* parent = nullptr);
    ~LevelTreeNode();
    size_t GetSize() const;  //统计挂载的叶子节点个数
    // operator==(LevelTreeNode<N, L>* pNodePrev, LevelTreeNode<N, L>* pNodeNext);
    // operator<(LevelTreeNode<N, L>* pNodePrev, LevelTreeNode<N, L>* pNodeNext);
    // operator>(LevelTreeNode<N, L>* pNodePrev, LevelTreeNode<N, L>* pNodeNext);
    bool IsLeaf() const;
    // 数据成员
    LevelTreeNode<N, L>* parent;
    union Keys {
        N non_leaf;
        L* leaf;  // 使用联合体可以减少子类声明的数量
        Keys(N non_leaf) : non_leaf(non_leaf) {}
        Keys(L* leaf) : leaf(leaf) {}
        ~Keys() {}
    } keys;

    bool is_leaf;  // 是否是叶子节点
    std::vector<LevelTreeNode<N, L>*> children;
};

template <typename N, typename L>
class LevelTree {
   public:
    LevelTree();
    void Insert(std::vector<N>& noneLeafNodes, L* leafNode);
    void Insert(std::vector<LevelTreeNode<N, L>>& nodes);
    std::vector<L> Query(std::vector<N>& nodeLeafNode);
    void BFSQuery(std::vector<N>& nonLeafNodes, std::vector<L>& result);
    size_t GetSize() const;
    void Print();
    void DFSPrint(LevelTreeNode<N, L>* node);  // 深度遍历打印树结构

   private:
    LevelTreeNode<N, L>* root;
};

template <typename N, typename L>
bool LevelTreeNode<N, L>::IsLeaf() const {
    return is_leaf;
}

template <typename N, typename L>
LevelTreeNode<N, L>::LevelTreeNode(N non_leaf_node, LevelTreeNode<N, L>* parent)
    : parent(parent), keys(non_leaf_node), is_leaf(false) {}

template <typename N, typename L>
LevelTreeNode<N, L>::LevelTreeNode(L* leaf_node, LevelTreeNode<N, L>* parent)
    : parent(parent), keys(leaf_node), is_leaf(true) {}

template <typename N, typename L>
LevelTreeNode<N, L>::~LevelTreeNode() {
    for (LevelTreeNode<N, L>* child : children) {
        if (child) {
            delete child;
        }
    }
}

// template <typename N, typename L>
// bool LevelTreeNode<N, L>::operator==(LevelTreeNode<N, L>* pNodePrev,
//                                      LevelTreeNode<N, L>* pNodeNext) const {}

// template <typename N, typename L>
// bool LevelTreeNode<N, L>::operator<(LevelTreeNode<N, L>* pNodePrev,
//                                     LevelTreeNode<N, L>* pNodeNext) const {}

// template <typename N, typename L>
// bool LevelTreeNode<N, L>::operator>(LevelTreeNode<N, L>* pNodePrev,
//                                     LevelTreeNode<N, L>* pNodeNext) const {}

/**
 * @todo 获取节点下挂载的所有叶子节点
 * @example 统计某个行业下所有的上市公司数量
 */
template <typename N, typename L>
size_t LevelTreeNode<N, L>::GetSize() const {
    std::queue<LevelTreeNode<N, L>*> queue;
    size_t n = 0;
    // 将起始节点入队
    queue.push(this);
    while (!queue.empty()) {
        // 取出队头节点
        LevelTreeNode<N, L>* current = queue.front();
        queue.pop();
        if (!current.IsLeaf()) {  // 非叶子节点
            for (size_t i = 0; i < current.children.size(); i++) {
                queue.push(current.children[i]);
            }
        } else {  // 叶子节点
            n++;
        }
    }
    return n;
}

template <typename N, typename L>
LevelTree<N, L>::LevelTree() : root(nullptr) {}

template <typename N, typename L>
void LevelTree<N, L>::Insert(std::vector<N>& nonLeafNodes, L* leafNode) {
    if (nonLeafNodes.size() == 0) {  // 检查参数是否为空
        return;
    }
    if (!root) {  // 检查根节点是否为空
        root = new LevelTreeNode<N, L>("root");
        root->keys.non_leaf = {};
        root->is_leaf = false;
        root->parent = nullptr;
    }
    // 插入剩下的节点
    LevelTreeNode<N, L>* current = root;

    for (size_t i = 0; i < nonLeafNodes.size(); i++) {  // 插入非叶子节点
        N non_leaf = nonLeafNodes[i];
        bool node_exist = false;
        for (size_t j = 0; j < current->children.size();
             ++j) {  // 检查非叶子节点是否已存在，如果是，继续迭代到下一层，
            if (non_leaf == current->children[j]->keys.non_leaf) {
                current = current->children[j];
                node_exist = true;
                break;  // 找到了继续迭代下一级
            }
        }
        if (!node_exist) {
            // 在相应层级上没有找到相关非叶子节点，添加一个
            LevelTreeNode<N, L>* non_tree_node = new LevelTreeNode<N, L>(non_leaf);
            non_tree_node->parent = current;
            current->children.push_back(non_tree_node);
            current = non_tree_node;
        }
    }

    LevelTreeNode<N, L>* leaf_tree_node = new LevelTreeNode<N, L>(leafNode);
    leaf_tree_node->parent = current;
    current->children.push_back(leaf_tree_node);
}

/**
 * @todo 广度优先搜索指定路径下的所有叶子节点
 * @param nonLeafNodes 非叶子节点路径，逆序排列
 */
template <typename N, typename L>
void LevelTree<N, L>::BFSQuery(std::vector<N>& nonLeafNodePath, std::vector<L>& result) {
    // 先遍历节点路径
    LevelTreeNode<N, L>* current = root;
    for (size_t i = 0; i < nonLeafNodePath.size(); i++) {
        N non_leaf = nonLeafNodePath.pop_back();
        for (size_t j = 0; j < current.children.size(); j++) {
            if (current.chidren[j].keys.non_leaf == non_leaf) {
                current = current.children[j];
            }
        }
    }

    // 创建一个队列来存储待访问的节点
    std::queue<LevelTreeNode<N, L>*> queue;
    // 将起始节点入队
    queue.push(current);
    while (!queue.empty()) {
        // 取出队头节点
        LevelTreeNode<N, L>* current = queue.front();
        queue.pop();
        if (!current.IsLeaf()) {  // 非叶子节点
            for (size_t i = 0; i < current.children.size(); i++) {
                queue.push(current.children[i]);
            }
        } else {  // 叶子节点
            result.push_back(current.keys.leaf);
        }
    }
}

template <typename N, typename L>
std::vector<L> LevelTree<N, L>::Query(std::vector<N>& nonLeafNodePath) {
    std::vector<L> result;
    std::reverse(nonLeafNodePath.begin(), nonLeafNodePath.end());
    BFSQuery(nonLeafNodePath, result);
    return result;
}

template <typename N, typename L>
size_t LevelTree<N, L>::GetSize() const {
    return root.GetSize();
}

template <typename N, typename L>
void LevelTree<N, L>::Print() {
    for (size_t i = 0; i < root->children.size(); i++) {
        DFSPrint(root->children[i]);
    }
}

template <typename N, typename L>
void LevelTree<N, L>::DFSPrint(LevelTreeNode<N, L>* node) {
    if (node->IsLeaf()) {
        std::vector<N> parents;
        LevelTreeNode<N, L>* current = node->parent;
        while (current && current->keys.non_leaf != "") {
            parents.push_back(current->keys.non_leaf);
            current = current->parent;
        }

        for (auto it = parents.rbegin(); it != parents.rend(); ++it) {
            if (it != parents.rend() - 1) {
                std::cout << *it << "-";
            } else {
                std::cout << *it;
            }
        }
        std::cout << "," + node->keys.leaf->name << std::endl;
    }

    for (LevelTreeNode<N, L>* child_node : node->children) {
        DFSPrint(child_node);
    }
}

#endif  // LevelTee_H