/**
* implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>

#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<class Key, class Tp, class Compare = std::less<Key>>
class map {
public:
  typedef pair<const Key, Tp> value_type;
private:
  struct Node {
    enum class Color { Red, Black };

    Node *parent, *left, *right;
    value_type *value;
    Color color;

    Node(): parent(nullptr), left(nullptr), right(nullptr), value(nullptr), color(Color::Red) {}
    Node(const value_type &val): Node() {
      value = new value_type(val);
    }
    Node(const Node &other) = delete;
    Node(Node &&other) = delete;
    ~Node() {
      delete value;
    }

    void switch_color() {
      color = (color == Color::Red) ? Color::Black : Color::Red;
    }
  };

  // void_node for end/rend iterator
  Node *root_, *left_most_, *right_most_, *void_node_;
  size_t size_;
  Compare lesser_comparer_;

  void clear_tree(Node *node) {
    // node to delete
    // if(node == nullptr) return; // for the deconstruction of rvalue-moved map.
    if(node->left != nullptr) clear_tree(node->left);
    if(node->right != nullptr) clear_tree(node->right);
    delete node;
  }
  void copy_tree(Node *des, Node *src, const map &other) {
    // src already copied to des
    if(src->left != nullptr) {
      des->left = new Node(*src->left->value);
      des->left->parent = des;
      des->left->color = src->left->color;
      if(other.left_most_ == src->left) left_most_ = des->left;
      copy_tree(des->left, src->left);
    }
    if(src->right != nullptr) {
      des->right = new Node(*src->right->value);
      des->right->parent = des;
      des->right->color = src->right->color;
      if(other.right_most_ == src->right) right_most_ = des->right;
      copy_tree(des->right, src->right);
    }
  }

  void left_rotate(Node *node) {
    Node *child = node->right;
    child->parent = node->parent;
    if(child->parent == nullptr) root_ = child;
    else if(child->parent->left == node) child->parent->left = child;
    else child->parent->right = child;
    node->right = child->left;
    if(node->right != nullptr) node->right->parent = node;
    child->left = node;
    node->parent = child;
  }
  void right_rotate(Node *node) {
    Node *child = node->left;
    child->parent = node->parent;
    if(child->parent == nullptr) root_ = child;
    else if(child->parent->left == node) child->parent->left = child;
    else child->parent->right = child;
    node->left = child->right;
    if(node->left != nullptr) node->left->parent = node;
    child->right = node;
    node->parent = child;
  }

  // changes node to its next.
  // if node == right_most, changes to void_node.
  // if node == nullptr or node == void_node, (holds) changes it to nullptr.
  void get_next(Node *node) const {
    if(node == nullptr) return;
    if(node == void_node_) {
      node = nullptr;
      return;
    }
    if(node == right_most_) {
      node = void_node_;
      return;
    }
    // if node == root here, for node != right_most_, we must have node->right != nullptr.
    // so is in the loop.
    if(node->right == nullptr) {
      while(node->parent->right == node) node = node->parent;
      node = node->parent;
      return;
    }
    node = node->right;
    while(node->left != nullptr) node = node->left;
  }
  // changes node to its prev.
  // if node == left_most, changes to void_node.
  // if node == nullptr or node == void_node, (holds) changes it to nullptr.
  void get_prev(Node *node) const {
    if(node == nullptr) return;
    if(node == void_node_) {
      node = nullptr;
      return;
    }
    if(node == left_most_) {
      node = void_node_;
      return;
    }
    // if node == root here, for node != left_most_, we must have node->left != nullptr.
    // so is in the loop.
    if(node->left == nullptr) {
      while(node->parent->left == node) node = node->parent;
      node = node->parent;
      return;
    }
    node = node->left;
    while(node->right != nullptr) node = node->right;
  }

  void insertion_maintain(Node *&node);
  void erasure_maintain(Node *&node);



public:
  class const_iterator;
  class iterator {
  private:
    const map<Key, Tp, Compare> *container;
    Node *node;

  public:
    iterator(): container(nullptr), node(nullptr) {}
    iterator(const map<Key, Tp, Compare> *the_map, Node *the_node): container(the_map), node(the_node) {}
    iterator(const iterator &other): container(other.container), node(other.node) {}
    iterator operator++(int) {
      iterator res = *this;
      ++(*this);
      return res;
    }
    iterator& operator++() {
      get_next(node);
      if(node == nullptr) // previous node = nullptr (default iterator) or node == void_node (end iterator)
        throw invalid_iterator();
      return *this;
    }
    iterator operator--(int) {
      iterator res = *this;
      --(*this);
      return res;
    }
    iterator& operator--() {
      get_prev(node);
      if(node == nullptr) // previous node == nullptr (default iterator) or node == void_node (end iterator)
        throw invalid_iterator();
      return *this;
    }
    bool operator==(const iterator &other) const {
      return container == other.container && node == other.node;
    }
    bool operator==(const const_iterator &other) const {
      return container == other.container && node == other.node;
    }
    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }
    bool operator!=(const const_iterator &other) const {
      return !(*this == other);
    }
    value_type& operator*() const {
      return *node->value;
    }
    value_type* operator->() const {
      return &*(*this);
    }
  };
  class const_iterator {
  private:
    const map<Key, Tp, Compare> *container;
    Node *node;

  public:
    const_iterator(): container(nullptr), node(nullptr) {}
    const_iterator(const map<Key, Tp, Compare> *the_map, Node *the_node): container(the_map), node(the_node) {}
    const_iterator(const const_iterator &other): container(other.container), node(other.node) {}
    const_iterator(const iterator &other): container(other.container), node(other.node) {}
    const_iterator operator++(int) {
      const_iterator res = *this;
      ++(*this);
      return res;
    }
    const_iterator& operator++() {
      get_next(node);
      if(node == nullptr) // previous node = nullptr (default iterator) or node == void_node (end iterator)
        throw invalid_iterator();
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator res = *this;
      --(*this);
      return res;
    }
    const_iterator& operator--() {
      get_prev(node);
      if(node == nullptr) // previous node == nullptr (default iterator) or node == void_node (end iterator)
        throw invalid_iterator();
      return *this;
    }
    bool operator==(const iterator &other) const {
      return container == other.container && node == other.node;
    }
    bool operator==(const const_iterator &other) const {
      return container == other.container && node == other.node;
    }
    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }
    bool operator!=(const const_iterator &other) const {
      return !(*this == other);
    }
    const value_type& operator*() const {
      return *node->value;
    }
    const value_type* operator->() const {
      return &*(*this);
    }
  };

  map(): root_(nullptr), void_node_(new Node), size_(0) {
    root_ = left_most_ = right_most_ = void_node_;
  }
  map(const map &other): map() {
    if(other.empty()) return;
    size_ = other.size_;
    root_ = new Node(*other.root_->value);
    root_->color =  other.root_->color;
    if(other.left_most_ == other.root_) left_most_ = root_;
    if(other.right_most_ == other.root_) right_most_ = root_;
    copy_tree(root_, other.root_, other);
  }
  map(map &&other) {
    size_ = other.size_;
    root_ = other.root_;
    left_most_ = other.left_most_;
    right_most_ = other.right_most_;
    void_node_ = other.right_most_;
  }
  ~map() {
    if(void_node_ == nullptr) return; // rvalue-moved
    clear();
    delete void_node_;
  }
  map& operator=(const map &other) {
    clear();
    if(other.empty()) return *this;
    size_ = other.size_;
    root_ = new Node(*other.root_->value);
    root_->color =  other.root_->color;
    if(other.left_most_ == other.root_) left_most_ = root_;
    if(other.right_most_ == other.root_) right_most_ = root_;
    copy_tree(root_, other.root_, other);
    return *this;
  }
  map& operator=(map &&other) {
    clear();
    size_ = other.size_;
    root_ = other.root_;
    left_most_ = other.left_most_;
    right_most_ = other.right_most_;
    void_node_ = other.right_most_;
    return *this;
  }
  // when empty(), begin() == end().
  iterator begin() {
    return iterator(this, left_most_);
  }
  iterator end() {
    return iterator(this, void_node_);
  }
  // when empty(), cbegin() == cend().
  const_iterator cbegin() {
    return const_iterator(this, left_most_);
  }
  const_iterator cend() {
    return const_iterator(this, void_node_);
  }
  void clear() {
    if(empty()) return;
    size_ = 0;
    clear_tree(root_);
    root_ = left_most_ = right_most_ = void_node_;
  }
  size_t size() const {
    return size_;
  }
  bool empty() const {
    return size_ == 0;
  }
  // returns end iterator if search fails.
  iterator find(const Key &key) {
    if(empty()) return end();
    Node *node = root_;
    while(node != nullptr) {
      if(lesser_comparer_(key, node->value->first))
        node = node->left;
      else if(lesser_comparer_(node->value->first, key))
        node = node->right;
      else return iterator(this, node);
    }
    return end();
  }
  const_iterator find(const Key &key) const {
    if(empty()) return cend();
    Node *node = root_;
    while(node != nullptr) {
      if(lesser_comparer_(key, node->value->first))
        node = node->left;
      else if(lesser_comparer_(node->value->first, key))
        node = node->right;
      else return const_iterator(this, node);
    }
    return cend();
  }
  size_t count(const Key &key) const {
    return (find(key) == cend()) ? 0 : 1;
  }
  Tp& at(const Key &key) {
    iterator it = find(key);
    if(it == end()) throw index_out_of_bound();
    return it->second;
  }
  const Tp& at(const Key &key) const {
    const_iterator it = find(key);
    if(it == cend()) throw index_out_of_bound();
    return it->second;
  }
  // insert an empty Tp value into the map.
  Tp& operator[](const Key &key) {
    static_assert(std::is_default_constructible<Tp>::value,
      "The type of value (Tp) should be default constructible if you want to use non-const operator[]");
    if(empty()) {
      size_ = 1;
      root_ = left_most_ = right_most_ = new Node(std::make_pair(key, Tp()));
      return root_->value->second;
    }
    Node *node = root_, *parent = nullptr;
    bool is_left = true;
    // at lease one comparison would be done.
    while(node != nullptr) {
      if(lesser_comparer_(key, node->value->first)) {
        parent = node;
        node = node->left;
        is_left = true;
      } else if(lesser_comparer_(node->value->first, key)) {
        parent = node;
        node = node->right;
        is_left = false;
      } else return node->value->second;
    }
    Node *res = new Node(std::make_pair(key, Tp()));
    res->parent = parent;
    if(is_left) {
      parent->left = res;
      if(parent == left_most_) left_most_ = res;
    } else {
      parent->right = res;
      if(parent == right_most_) right_most_ = res;
    }
    insertion_maintain(res);
    return res->value->second;
  }
  // throws index_out_of_bound if key doesn't exist.
  const Tp& operator[](const Key &key) const {
    const_iterator it = find(key);
    if(it == cend()) throw index_out_of_bound();
    return it->second;
  }
  pair<iterator, bool> insert(const value_type &value) {
    if(empty()) {
      size_ = 1;
      root_ = left_most_ = right_most_ = new Node(value);
      return std::make_pair(iterator(this, root_), true);
    }
    Node *node = root_, *parent = nullptr;
    bool is_left = true;
    // at lease one comparison would be done.
    while(node != nullptr) {
      if(lesser_comparer_(value.first, node->value->first)) {
        parent = node;
        node = node->left;
        is_left = true;
      } else if(lesser_comparer_(node->value->first, value.first)) {
        parent = node;
        node = node->right;
        is_left = false;
      } else return std::make_pair(iterator(this, node), false);
    }
    Node *res = new Node(value);
    res->parent = parent;
    if(is_left) {
      parent->left = res;
      if(parent == left_most_) left_most_ = res;
    } else {
      parent->right = res;
      if(parent == right_most_) right_most_ = res;
    }
    insertion_maintain(res);
    return std::make_pair(iterator(this, res), true);
  }
  void erase(iterator pos);
};
}

#endif