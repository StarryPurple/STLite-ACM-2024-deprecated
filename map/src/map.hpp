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

    Node *parent {}, *left {}, *right {};
    value_type value;
    Color color;

    Node(const value_type &value_): value(value_), color(Color::Red) {}
    Node(const Node &other) = delete;
    Node(Node &&other) = delete;
  };

  Node *root_, *left_most_, *right_most_;
  size_t size_;
  Compare lesser_comparer_;

  void self_check(Node *node) {
    if(node == nullptr) return;
    if(node->left != nullptr) {
      assert(node->left->parent == node);
      self_check(node->left);
    }
    if(node->right != nullptr) {
      assert(node->right->parent == node);
      self_check(node->right);
    }
  }

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
      des->left = new Node(src->left->value);
      des->left->parent = des;
      des->left->color = src->left->color;
      if(other.left_most_ == src->left) left_most_ = des->left;
      copy_tree(des->left, src->left, other);
    }
    if(src->right != nullptr) {
      des->right = new Node(src->right->value);
      des->right->parent = des;
      des->right->color = src->right->color;
      if(other.right_most_ == src->right) right_most_ = des->right;
      copy_tree(des->right, src->right, other);
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
  Node* get_next(Node *node) const {
    if(node == nullptr) return left_most_;
    if(node == right_most_) return nullptr;
    // if node == root here, for node != right_most_, we must have node->right != nullptr.
    // so is in the loop.
    if(node->right == nullptr) {
      while(node->parent->right == node) node = node->parent;
      node = node->parent;
      return node;
    }
    node = node->right;
    while(node->left != nullptr) node = node->left;
    return node;
  }
  // changes node to its prev.
  Node* get_prev(Node *node) const {
    if(node == nullptr) return right_most_;
    if(node == left_most_) return nullptr;
    // if node == root here, for node != left_most_, we must have node->left != nullptr.
    // so is in the loop.
    if(node->left == nullptr) {
      while(node->parent->left == node) node = node->parent;
      node = node->parent;
      return node;
    }
    node = node->left;
    while(node->right != nullptr) node = node->right;
    return node;
  }

  void insertion_maintain(Node *node) {
    // This node should be red.
    // maintain upwards.

    // Case 1: the new node is the root / tree is previously empty.
    if(node == root_) {
      node->color = Node::Color::Black;
      return;
    }

    // node has parent.
    Node *parent = node->parent;
    // Case 2: parent is black.
    if(parent->color == Node::Color::Black) return;

    // parent is red.
    // Case 3: parent is red root.
    if(parent == root_) {
      parent->color = Node::Color::Black;
      return;
    }

    // node has grandparent (black).
    Node *grandparent = parent->parent;
    Node *uncle = (grandparent->left == parent) ? grandparent->right : grandparent->left;
    // Case 4: uncle node exists, and it's red.
    if(uncle != nullptr && uncle->color == Node::Color::Red) {
      parent->color = uncle->color = Node::Color::Black;
      grandparent->color = Node::Color::Red;
      insertion_maintain(grandparent);
      return;
    }

    // uncle node doesn't exist or is black.
    // Case 5: the direction of the two parent-child relationship isn't identical.
    if((grandparent->left == parent) ^ (parent->left == node)) {
      if(parent->left == node) right_rotate(parent);
      else left_rotate(parent);
      std::swap(node, parent);
      // Go on to case 6.
    }

    // Case 6: the direction of the two parent-child relationship is identical.
    parent->color = Node::Color::Black;
    grandparent->color = Node::Color::Red;
    if(grandparent->left == parent) right_rotate(grandparent);
    else left_rotate(grandparent);
  }
  void erasure_maintain(Node *node) {
    // The black length of parent of this node has just been shortened by 1.
    // the ancestors of this node might be affected.
    // maintain upwards.

    // Case 1: node is root.
    if(node == root_) return; // no actual node is affected.

    // node here has parent.
    Node *parent = node->parent;
    Node *sibling = (parent->left == node) ? parent->right : parent->left;
    // Check case: sibling not exist.
    if(sibling == nullptr) {
      // at least parent node is not affected.
      // other ancestors are still in need of maintenance.
      erasure_maintain(parent);
      return;
    }

    // Case 2: sibling node is red.
    if(sibling->color == Node::Color::Red) {
      // parent should be black.
      // children of sibling nodes should be black.
      parent->color = Node::Color::Red;
      sibling->color = Node::Color::Black;
      if(parent->left == sibling) {
        right_rotate(parent);
        sibling = parent->left;
      } else {
        left_rotate(parent);
        sibling = parent->right;
      }
      // now sibling node is black (or not exist).
      // continue to further maintenance.
    }

    // Check case: sibling not exist.
    if(sibling == nullptr) {
      // at least parent node is not affected.
      // other ancestors are still in need of maintenance.
      erasure_maintain(parent);
      return;
    }

    // Case 3: sibling has no red children
    if((sibling->left == nullptr || sibling->left->color == Node::Color::Black)
      && (sibling->right == nullptr || sibling->right->color == Node::Color::Black)) {
      sibling->color = Node::Color::Black;
      if(parent->color == Node::Color::Red) {
        parent->color = Node::Color::Black;
        return;
      } else {
        erasure_maintain(parent);
        return;
      }
    }

    // Case 4: sibling has at least one red children
    // Modify: make sibling's opposite-side (compared to node's side in parent) child is red.
    if(node == parent->left) {
      if(sibling->right == nullptr || sibling->right->color == Node::Color::Black) {
        // sibling->left->color == Red
        sibling->left->color = Node::Color::Black;
        sibling->color = Node::Color::Red;
        right_rotate(sibling);
        sibling = parent->right;
      }
      // now sibling->right->color = Red
      sibling->color = parent->color;
      parent->color = Node::Color::Black;
      sibling->right->color = Node::Color::Black;
      left_rotate(parent);
      return;
    } else {
      // node == parent_right
      if(sibling->left == nullptr || sibling->left->color == Node::Color::Black) {
        // sibling->right->color == Red
        sibling->right->color = Node::Color::Black;
        sibling->color = Node::Color::Red;
        left_rotate(sibling);
        sibling = parent->left;
      }
      // now sibling->left->color = Red
      sibling->color = parent->color;
      parent->color = Node::Color::Black;
      sibling->left->color = Node::Color::Black;
      right_rotate(parent);
      return;
    }
  }



public:
  class const_iterator;
  class iterator {
    friend void sjtu::map<Key, Tp, Compare>::erase(iterator pos);
    friend const_iterator;
  private:
    const map<Key, Tp, Compare> *container;
    Node *node;

  public:
    iterator(): container(nullptr), node(nullptr) {} // default iterator as end()
    iterator(const map<Key, Tp, Compare> *the_map, Node *the_node): container(the_map), node(the_node) {}
    iterator(const iterator &other): container(other.container), node(other.node) {}
    iterator& operator=(const iterator &other) = default;
    iterator& operator=(iterator &&other) = default;
    iterator operator++(int) {
      iterator res = *this;
      ++(*this);
      return res;
    }
    iterator& operator++() {
      if(node == nullptr) // end()
        throw invalid_iterator();
      Node *next = container->get_next(node);
      node = next;
      return *this;
    }
    iterator operator--(int) {
      iterator res = *this;
      --(*this);
      return res;
    }
    iterator& operator--() {
      if(node == container->left_most_) // begin()
        throw invalid_iterator();
      Node *prev = container->get_prev(node);
      node = prev;
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
      return node->value;
    }
    value_type* operator->() const {
      return &*(*this);
    }
  };
  class const_iterator {
    friend iterator;
  private:
    const map<Key, Tp, Compare> *container;
    Node *node;

  public:
    const_iterator(): container(nullptr), node(nullptr) {} // default iterator as cend()
    const_iterator(const map<Key, Tp, Compare> *the_map, Node *the_node): container(the_map), node(the_node) {}
    const_iterator(const const_iterator &other): container(other.container), node(other.node) {}
    const_iterator(const iterator &other): container(other.container), node(other.node) {}
    const_iterator& operator=(const const_iterator &other) = default;
    const_iterator& operator=(const_iterator &&other) = default;
    const_iterator operator++(int) {
      const_iterator res = *this;
      ++(*this);
      return res;
    }
    const_iterator& operator++() {
      if(node == nullptr) // cend()
        throw invalid_iterator();
      Node *next = container->get_next(node);
      node = next;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator res = *this;
      --(*this);
      return res;
    }
    const_iterator& operator--() {
      if(node == container->left_most_) // cbegin()
        throw invalid_iterator();
      Node *prev = container->get_prev(node);
      node = prev;
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
      return node->value;
    }
    const value_type* operator->() const {
      return &*(*this);
    }
  };

  map(): root_(nullptr), size_(0) {
    root_ = nullptr;
    left_most_ = right_most_ = nullptr;
  }
  map(const map &other): map() {
    if(other.empty()) return;
    size_ = other.size_;
    root_ = new Node(other.root_->value);
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
  }
  ~map() {
    clear();
  }
  map& operator=(const map &other) {
    if(this == &other) return *this;
    clear();
    if(other.empty()) return *this;
    size_ = other.size_;
    root_ = new Node(other.root_->value);
    root_->color =  other.root_->color;
    if(other.left_most_ == other.root_) left_most_ = root_;
    if(other.right_most_ == other.root_) right_most_ = root_;
    copy_tree(root_, other.root_, other);
    return *this;
  }
  map& operator=(map &&other) {
    if(this == &other) return *this;
    clear();
    size_ = other.size_;
    root_ = other.root_;
    left_most_ = other.left_most_;
    right_most_ = other.right_most_;
    return *this;
  }
  // when empty(), begin() == end().
  iterator begin() {
    return iterator(this, left_most_);
  }
  iterator end() {
    return iterator(this, nullptr);
  }
  // when empty(), cbegin() == cend().
  const_iterator cbegin() const {
    return const_iterator(this, left_most_);
  }
  const_iterator cend() const {
    return const_iterator(this, nullptr);
  }
  void clear() {
    if(empty()) return;
    clear_tree(root_);
    size_ = 0;
    root_ = nullptr;
    left_most_ = right_most_ = nullptr;
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
      if(lesser_comparer_(key, node->value.first))
        node = node->left;
      else if(lesser_comparer_(node->value.first, key))
        node = node->right;
      else return iterator(this, node);
    }
    return end();
  }
  const_iterator find(const Key &key) const {
    if(empty()) return cend();
    Node *node = root_;
    while(node != nullptr) {
      if(lesser_comparer_(key, node->value.first))
        node = node->left;
      else if(lesser_comparer_(node->value.first, key))
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
      root_ = left_most_ = right_most_ = new Node(value_type(key, Tp()));
      return root_->value.second;
    }
    Node *node = root_, *parent = nullptr;
    bool is_left = true;
    // at lease one comparison would be done.
    while(node != nullptr) {
      if(lesser_comparer_(key, node->value.first)) {
        parent = node;
        node = node->left;
        is_left = true;
      } else if(lesser_comparer_(node->value.first, key)) {
        parent = node;
        node = node->right;
        is_left = false;
      } else return node->value.second;
    }
    ++size_;
    Node *res = new Node(value_type(key, Tp()));
    res->parent = parent;
    if(is_left) {
      parent->left = res;
      if(parent == left_most_) left_most_ = res;
    } else {
      parent->right = res;
      if(parent == right_most_) right_most_ = res;
    }
    insertion_maintain(res);
    return res->value.second;
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
      return pair<iterator, bool>(iterator(this, root_), true);
    }
    Node *node = root_, *parent = nullptr;
    bool is_left = true;
    // at lease one comparison would be done.
    while(node != nullptr) {
      if(lesser_comparer_(value.first, node->value.first)) {
        parent = node;
        node = node->left;
        is_left = true;
      } else if(lesser_comparer_(node->value.first, value.first)) {
        parent = node;
        node = node->right;
        is_left = false;
      } else return pair<iterator, bool>(iterator(this, node), false);
    }
    ++size_;
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
    return pair<iterator, bool>(iterator(this, res), true);
  }
  // may throw invalid_iterator if invalidation is detected.
  // (visiting deleted pointer may occur, resulting in core dump(?).)
  void erase(iterator pos) {
    // I assume this pos is a valid iterator that are in use (and, including end() and rend() for now).
    if(pos.container != this || empty() || pos == end()) throw invalid_iterator();
    if(size_ == 1) {
      if(pos.node != root_) throw invalid_iterator();
      delete root_;
      root_ = nullptr;
      left_most_ = right_most_ = nullptr;
      size_ = 0;
      return;
    }
    --size_;
    Node *node = pos.node;
    if(node == right_most_) right_most_ = get_prev(node);
    if(node == left_most_) left_most_ = get_next(node);
    if(node->left != nullptr && node->right != nullptr) {
      Node *prev = get_prev(node);
      // This approach works, but it breaks the legality of iterator to prev.
      /*
      delete node->value;
      node->value = new value_type(*prev->value);
      if(prev == left_most_) left_most_ = node;
      node = prev;
      */
      if(node->left == prev) {
        // prev->right == nullptr
        right_rotate(node);
        // now node->left == nullptr
      } else {
        Node *node_parent = node->parent, *node_left = node->left, *node_right = node->right;
        Node *prev_parent = prev->parent, *prev_left = prev->left, *prev_right = prev->right;

        node->parent = prev_parent; node->left = prev_left; node->right = prev_right;
        prev->parent = node_parent; prev->left = node_left; prev->right = node_right;
        auto color = node->color; node->color = prev->color; prev->color = color;

        if(node_parent != nullptr) {
          if(node_parent->left == node) node_parent->left = prev;
          else node_parent->right = prev;
        }
        if(node_left != nullptr) node_left->parent = prev;
        if(node_right != nullptr) node_right->parent = prev;
        if(prev_parent != nullptr) {
          if(prev_parent->left == prev) prev_parent->left = node;
          else prev_parent->right = node;
        }
        if(prev_left != nullptr) prev_left->parent = node;
        if(prev_right != nullptr) prev_right->parent = node;
        if(root_ == node) root_ = prev; // Don't forget this.
      }
      // now (node->left == nullptr || node->right == nullptr) is true.
    }
    // no two-child node here.
    bool to_maintain = (node->color == Node::Color::Black);
    if(node->left == nullptr && node->right == nullptr) {
      // discard it.
      Node *parent = node->parent; // definitely not nullptr, for size_ == 1 case has been handled.
      if(to_maintain) {
        // temporarily use a new nil node whose parent is "parent".
        Node *helper_node = new Node(parent->value);
        helper_node->color = Node::Color::Black; // somehow needless
        helper_node->parent = parent;
        if(parent->left == node) {
          parent->left = helper_node;
          erasure_maintain(helper_node);
          parent->left = nullptr;
        } else {
          parent->right = helper_node;
          erasure_maintain(helper_node);
          parent->right = nullptr;
        }
        helper_node->parent = nullptr;
        delete helper_node;
      } else {
        if(parent->left == node) parent->left = nullptr;
        else parent->right = nullptr;
      }
      delete node;
      return;
    }
    Node *parent = node->parent; // may be nullptr if node == root_
    Node *child = (node->left == nullptr) ? node->right : node->left; // only one side is not nullptr.
    if(parent == nullptr) root_ = child;
    else if(parent->left == node) parent->left = child;
    else parent->right = child;
    child->parent = parent;
    child->color = node->color; // important?
    erasure_maintain(child);
    delete node;
  }
};
}

#endif