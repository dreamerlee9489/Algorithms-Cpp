#ifndef BINARY_TREE_H
#define BINARY_TREE_H
#include <iostream>
#include <memory>
#include <queue>
#include <cmath>
#include "./IString.h"
// 二叉树基类
template <typename T>
class IBinaryTree
{
    using traverse_func = void (*)(std::shared_ptr<T> data);

protected:
    template <typename U>
    struct Node : public IString
    {
        friend std::ostream &operator<<(std::ostream &os, const Node<U> &node) { return os << node.to_string(); }
        std::shared_ptr<U> _data = nullptr;
        Node<U> *_parent = nullptr, *_left = nullptr, *_right = nullptr;
        Node<U> &operator=(const Node<U> &node);
        Node<U> &operator=(Node<U> &&node);
        Node(std::shared_ptr<U> data, Node<U> *parent = nullptr, Node<U> *left = nullptr, Node<U> *right = nullptr)
            : _data(data), _parent(parent), _left(left), _right(right) {}
        virtual ~Node() { _data = nullptr; }
        Node(const Node<U> &node) { *this = node; }
        Node(Node<U> &&node) { *this = std::move(node); }
        bool is_leaf() const { return _left == nullptr && _right == nullptr; }
        bool is_binary() const { return _left != nullptr && _right != nullptr; }
        bool is_left() const { return _parent != nullptr && this == _parent->_left; }
        bool is_right() const { return _parent != nullptr && this == _parent->_right; }
        Node<U> *get_sibling() const;
        std::string to_string() const override { return ((IString &)*_data).to_string(); }
    };
    size_t _size = 0;
    virtual Node<T> *create_node(std::shared_ptr<T> data, Node<T> *parent) { return new Node<T>(data, parent); }
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
    virtual ~IBinaryTree() { clear_recu(_root); }
    virtual void add(std::shared_ptr<T> data) = 0;
    virtual void remove(std::shared_ptr<T> data) = 0;
    size_t size() const { return _size; }
    size_t height() const { return height_iter(_root); }
    bool is_empty() const { return _size == 0; }
    bool is_complete() const;
    bool contains(std::shared_ptr<T> data) const { return get_node(data) != nullptr; }
    void traverse(TraverseOrder order = TraverseOrder::In, traverse_func func = nullptr) const;
    void clear();
};

template <typename T>
template <typename U>
inline IBinaryTree<T>::Node<U> &IBinaryTree<T>::Node<U>::operator=(const IBinaryTree<T>::Node<U> &node)
{
    _data = node._data;
    _parent = node._parent;
    _left = node._left;
    _right = node._right;
    return *this;
}

template <typename T>
template <typename U>
inline IBinaryTree<T>::Node<U> &IBinaryTree<T>::Node<U>::operator=(Node<U> &&node)
{
    _data = std::move(node._data);
    _parent = std::move(node._parent);
    _left = std::move(node._left);
    _right = std::move(node._right);
    return *this;
}

template <typename T>
template <typename U>
inline IBinaryTree<T>::Node<U> *IBinaryTree<T>::Node<U>::get_sibling() const
{
    if (is_left())
        return _parent->_right;
    else if (is_right())
        return _parent->_left;
    return nullptr;
}

template <typename T>
inline size_t IBinaryTree<T>::height_recu(Node<T> *node) const
{
    if (node == nullptr)
        return 0;
    return 1 + std::max(height_recu(node->_left), height_recu(node->_right));
}

template <typename T>
inline size_t IBinaryTree<T>::height_iter(Node<T> *node) const
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
inline bool IBinaryTree<T>::is_complete() const
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

        if (elem->_left != nullptr)
            q.push(elem->_left);
        else if (elem->_right != nullptr)
            return false;

        if (elem->_right != nullptr)
            q.push(elem->_right);
        else
            leaf = true; //最后一个非叶结点结点不入队
    }
    return true;
}

template <typename T>
inline IBinaryTree<T>::Node<T> *IBinaryTree<T>::get_predecessor(Node<T> *node) const
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
inline IBinaryTree<T>::Node<T> *IBinaryTree<T>::get_successor(Node<T> *node) const
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
inline void IBinaryTree<T>::clear_recu(Node<T> *node)
{
    if (node != nullptr)
    {
        clear_recu(node->_left);
        clear_recu(node->_right);
        delete node;
    }
}

template <typename T>
inline void IBinaryTree<T>::clear()
{
    clear_recu(_root);
    _root = nullptr;
    _size = 0;
}

template <typename T>
inline void IBinaryTree<T>::not_null_check(std::shared_ptr<T> data) const
{
    if (data == nullptr)
        throw std::invalid_argument("data must be not null.");
}

template <typename T>
inline void IBinaryTree<T>::traverse(TraverseOrder order, traverse_func func) const
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
inline void IBinaryTree<T>::inorder_traverse(Node<T> *node, traverse_func func) const
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
inline void IBinaryTree<T>::preorder_traverse(Node<T> *node, traverse_func func) const
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
inline void IBinaryTree<T>::postorder_traverse(Node<T> *node, traverse_func func) const
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
inline void IBinaryTree<T>::levelorder_traverse(Node<T> *node, traverse_func func) const
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