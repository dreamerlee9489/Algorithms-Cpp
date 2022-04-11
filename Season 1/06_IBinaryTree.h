#ifndef BINARY_TREE_H
#define BINARY_TREE_H
#include <iostream>
#include <memory>
#include <queue>
#include <cmath>
#include "./IString.h"
// 二叉树基类(智能指针版)
template <typename T>
class IBinaryTree
{
    // typedef void traverse_func(std::shared_ptr<T> data);
    using traverse_func = void (*)(std::shared_ptr<T> data);
    friend std::ostream &operator<<(std::ostream &os, const IBinaryTree<T> &tree) { return draw_tree(os, tree); }

protected:
    template <typename U>
    struct Node
    {
        std::shared_ptr<U> _data;
        Node<U> *_parent, *_left, *_right;
        Node(std::shared_ptr<U> data, Node<U> *parent = nullptr, Node<U> *left = nullptr, Node<U> *right = nullptr)
            : _data(data), _parent(parent), _left(left), _right(right) {}
        ~Node() { _data = nullptr; }
        bool is_leaf() const { return _left == nullptr && _right == nullptr; }
        bool is_binary() const { return _left != nullptr && _right != nullptr; }
    };
    size_t _size = 0;
    void not_null_check(std::shared_ptr<T> data) const;
    Node<T> *get_predecessor(Node<T> *node) const;
    Node<T> *get_successor(Node<T> *node) const;
    void inorder_traverse(Node<T> *node, traverse_func func) const;
    void preorder_traverse(Node<T> *node, traverse_func func) const;
    void postorder_traverse(Node<T> *node, traverse_func func) const;
    void levelorder_traverse(Node<T> *node, traverse_func func) const;
    size_t height_recu(Node<T> *node) const;
    size_t height_iter(Node<T> *node) const;
    void clear_recu(Node<T> *node);
    static std::ostream &draw_tree(std::ostream &os, const IBinaryTree<T> &tree);

public:
    enum class TraverseOrder
    {
        In,
        Pre,
        Post,
        Level
    };
    Node<T> *_root = nullptr;
    IBinaryTree() = default;
    ~IBinaryTree() { clear_recu(_root); }
    virtual Node<T> *get_node(std::shared_ptr<T> data) const = 0;
    virtual void add(std::shared_ptr<T> data) = 0;
    virtual void remove(std::shared_ptr<T> data) = 0;
    size_t size() const { return _size; }
    size_t height() const { return height_iter(_root); }
    bool is_empty() const { return _size == 0; }
    bool is_complete() const;
    bool contains(std::shared_ptr<T> data) const { return get_node(data) != nullptr; }
    void traverse(TraverseOrder order = TraverseOrder::In, traverse_func func = nullptr) const;
    void clear() { clear_recu(_root); }
};

template <typename T>
std::ostream &IBinaryTree<T>::draw_tree(std::ostream &os, const IBinaryTree<T> &tree)
{
    if (tree._root == nullptr)
        return os;
    size_t height = 0, total_height = tree.height();
    size_t level_count = 1;
    size_t str_size = 16;
    size_t width = std::pow(2, total_height - 1) * str_size;
    std::queue<typename IBinaryTree::template Node<T> *> q = std::queue<typename IBinaryTree::template Node<T> *>();
    q.push(tree._root);
    while (!q.empty())
    {
        size_t space = width / std::pow(2, height + 1) - str_size / 2;
        typename IBinaryTree::template Node<T> *elem = q.front();
        std::string str;
        if (elem != nullptr)
            str = std::string(space, ' ') + ((IString &)*elem->_data).to_string() + std::string(space, ' ');
        else
            str = std::string(str_size, ' ');
        os << str;
        q.pop();
        if (elem != nullptr)
            q.push(elem->_left);
        if (elem != nullptr)
            q.push(elem->_right);
        level_count--;
        if (level_count == 0)
        {
            level_count = q.size();
            height++;
            os << "\n";
        }
    }
    return os;
}

template <typename T>
size_t IBinaryTree<T>::height_recu(Node<T> *node) const
{
    if (node == nullptr)
        return 0;
    return 1 + std::max(height_recu(node->_left), height_recu(node->_right));
}

template <typename T>
size_t IBinaryTree<T>::height_iter(Node<T> *node) const
{
    if (node == nullptr)
        return 0;
    size_t height = 0, level_count = 1;
    std::queue<Node<T> *> q = std::queue<Node<T> *>();
    q.push(node);
    while (!q.empty())
    {
        Node<T> *elem = q.front();
        q.pop();
        level_count--;
        if (elem->_left != nullptr)
            q.push(elem->_left);
        if (elem->_right != nullptr)
            q.push(elem->_right);
        if (level_count == 0)
        {
            level_count = q.size();
            height++;
        }
    }
    return height;
}

template <typename T>
bool IBinaryTree<T>::is_complete() const
{
    if (_root == nullptr)
        return false;
    std::queue<Node<T> *> q = std::queue<Node<T> *>();
    q.push(_root);
    bool leaf = false;
    while (!q.empty())
    {
        Node<T> *elem = q.front();
        q.pop();
        if (leaf && !elem->is_leaf())
            return false;
        if (elem->is_binary())
        {
            q.push(elem->_left);
            q.push(elem->_right);
        }
        else if (elem->_left == nullptr && elem->_right != nullptr)
            return false;
        else
            leaf = true;
    }
    return true;
}

template <typename T>
IBinaryTree<T>::Node<T> *IBinaryTree<T>::get_predecessor(Node<T> *node) const
{
    if (node == nullptr)
        return nullptr;
    Node<T> *p = node->_left;
    if (p != nullptr)
    {
        while (p->_right != nullptr)
            p = p->_right;
        return p;
    }
    while (node->_parent != nullptr && node == node->_parent->_left)
        node = node->_parent;
    return node->_parent;
}

template <typename T>
IBinaryTree<T>::Node<T> *IBinaryTree<T>::get_successor(Node<T> *node) const
{
    if (node == nullptr)
        return nullptr;
    Node<T> *p = node->_right;
    if (p != nullptr)
    {
        while (p->_left != nullptr)
            p = p->_left;
        return p;
    }
    while (node->_parent != nullptr && node == node->_parent->_right)
        node = node->_parent;
    return node->_parent;
}

template <typename T>
void IBinaryTree<T>::clear_recu(Node<T> *node)
{
    if (node != nullptr)
    {
        clear_recu(node->_left);
        clear_recu(node->_right);
        delete node;
        _root = nullptr;
    }
}

template <typename T>
void IBinaryTree<T>::not_null_check(std::shared_ptr<T> data) const
{
    if (data == nullptr)
        throw std::invalid_argument("data must be not null.");
}

template <typename T>
void IBinaryTree<T>::traverse(TraverseOrder order, traverse_func func) const
{
    switch (order)
    {
    case TraverseOrder::In:
        inorder_traverse(_root, func);
        break;
    case TraverseOrder::Pre:
        preorder_traverse(_root, func);
        break;
    case TraverseOrder::Post:
        postorder_traverse(_root, func);
        break;
    case TraverseOrder::Level:
        levelorder_traverse(_root, func);
        break;
    }
}

template <typename T>
void IBinaryTree<T>::inorder_traverse(Node<T> *node, traverse_func func) const
{
    if (node != nullptr)
    {
        inorder_traverse(node->_left, func);
        if (func != nullptr)
            func(node->_data);
        else
            std::cout << *node->_data << "\n";
        inorder_traverse(node->_right, func);
    }
}

template <typename T>
void IBinaryTree<T>::preorder_traverse(Node<T> *node, traverse_func func) const
{
    if (node != nullptr)
    {
        if (func != nullptr)
            func(node->_data);
        else
            std::cout << *node->_data << "\n";
        preorder_traverse(node->_left, func);
        preorder_traverse(node->_right, func);
    }
}

template <typename T>
void IBinaryTree<T>::postorder_traverse(Node<T> *node, traverse_func func) const
{
    if (node != nullptr)
    {
        postorder_traverse(node->_left, func);
        postorder_traverse(node->_right, func);
        if (func != nullptr)
            func(node->_data);
        else
            std::cout << *node->_data << "\n";
    }
}

template <typename T>
void IBinaryTree<T>::levelorder_traverse(Node<T> *node, traverse_func func) const
{
    if (node == nullptr)
        return;
    std::queue<Node<T> *> q = std::queue<Node<T> *>();
    q.push(node);
    while (!q.empty())
    {
        Node<T> *elem = q.front();
        q.pop();
        if (elem->_left != nullptr)
            q.push(elem->_left);
        if (elem->_right != nullptr)
            q.push(elem->_right);
        if (func != nullptr)
            func(elem->_data);
        else
            std::cout << *elem->_data << "\n";
    }
}

#endif /* BINARY_TREE_H */