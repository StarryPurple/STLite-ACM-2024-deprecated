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

template<class Key, class T, class Compare = std::less<Key>>
class map {
public:
  typedef pair<const Key, T> value_type;
private:
  enum class NodeColor {
     kRed, kBlack
  };
  struct Node {
    value_type *val_ptr;
    Node *parent, *left, *right;
    NodeColor color;

    Node()
      : val_ptr(nullptr),
        parent(nullptr),
        left(nullptr), right(nullptr),
        color(NodeColor::kRed) {}
    Node(const value_type &value, Node *parent_ptr, Node *left_ptr, Node &right_ptr, NodeColor node_color)
      : val_ptr(new value_type(value)),
        parent(parent_ptr),
        left(left_ptr), right(right_ptr),
        color(node_color) {}
    ~Node() {
      delete val_ptr;
      val_ptr = nullptr;
      parent = left = right = nullptr;
    }
    Node *val_col_clone() const {
      return new Node(*val_ptr, nullptr, nullptr, nullptr, color);
    }
  };
  // sentinel_->left is leftmost (begin()), sentinel_->right is rightmost (rbegin()).
  // sentinel_->parent is root, root->parent is sentinel.
  // sentinel_ is reserved for iterator end() and rend().
  // when tree is empty, sentinel_->left = sentinel_->right = sentinel->parent = sentinel.
  size_t size_;
  Node *sentinel_;
  Compare comparer_;

  void left_rotate(Node *node) {
    Node *r_node = node->right;
    if(node->parent != nullptr) {
      if(node->parent->left == node) node->parent->left = r_node;
      else node->parent->right = r_node;
    }
    r_node->parent = node->parent;
    node->parent = r_node;
    node->right = r_node->left;
    r_node->left = node;
    if(node->right != nullptr) node->right->parent = node;
  }
  void right_rotate(Node *node) {
    Node *l_node = node->left;
    if(node->parent != nullptr) {
      if(node->parent->left == node) node->parent->left = l_node;
      else node->parent->right = l_node;
    }
    l_node->parent = node->parent;
    node->parent = l_node;
    node->left = l_node->right;
    l_node->right = node;
    if(node->left != nullptr) node->left->parent = node;
  }
  class iterator;
  void insert_maintain(Node *node) {
    // Case 1
    if(size_ == 1) return;
  }
  void erase_maintain(Node *node) {

  }

public:
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
  class const_iterator;
  class iterator {
  private:
    const map<Key, T, Compare> *container_;
    Node *node_; // nullptr for end().
  public:
    iterator(): container_(nullptr), node_(nullptr) {}
    iterator(const map<Key, T, Compare> &container, Node *node): container_(&container), node_(node) {}
    iterator(const iterator &other): container_(other.container_), node_(other.node_) {}
    iterator operator++(int) {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }
    iterator& operator++() {
      if(node_ == sentinel_) throw invalid_iterator(); // ++end() is invalid
      if(node_->right != nullptr) {
        node_ = node_->right;
        while(node_->left != nullptr) node_ = node_->left;
        return *this;
      }
      Node *parent = node_->parent;
      while(parent != sentinel_ && parent->right == node_) {
        // the order of conditions can't be swapped, or problem may occur when size_ == 1
        node_ = parent;
        parent = node_->parent;
      }
      node_ = parent;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      --*this;
      return tmp;
    }
    iterator& operator--() {
      if(node_ == sentinel_->left) throw invalid_iterator(); // --begin() is invalid
      if(node_->left != nullptr) {
        node_ = node_->left;
        while(node_->right != nullptr) node_ = node_->right;
        return *this;
      }
      Node *parent = node_->parent;
      while(parent->left == node_) {
        node_ = parent;
        parent = node_->parent;
      }
      // parent shouldn't be sentinel, for *this can't be begin() here.
      node_ = parent;
      return *this;
    }
    value_type& operator*() const {
      return *node_->val_ptr;
    }
    bool operator==(const iterator &rhs) const {
      return container_ == rhs.container_ && node_ == rhs.node_;
    }
    bool operator==(const const_iterator &rhs) const {
      return container_ == rhs.container_ && node_ == rhs.node_;
    }
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type* operator->() const noexcept {
      return &**this;
    }
  };
  class const_iterator {
    // it should have similar member method as iterator.
    //  and it should be able to construct from an iterator.
  private:
    const map<Key, T, Compare> *container_;
    Node *node_;
  public:
    const_iterator(): container_(nullptr), node_(nullptr) {}
    const_iterator(const map<Key, T, Compare> &container, Node *node)
      : container_(&container), node_(node) {}
    const_iterator(const const_iterator &other)
      : container_(other.container_), node_(other.node_) {}
    const_iterator(const iterator &other)
      : container_(other.container_), node_(other.node_) {}
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++*this;
      return tmp;
    }
    const_iterator& operator++() {
      if(node_ == sentinel_) throw invalid_iterator(); // ++cend() is invalid
      if(node_->right != nullptr) {
        node_ = node_->right;
        while(node_->left != nullptr) node_ = node_->left;
        return *this;
      }
      Node *parent = node_->parent;
      while(parent != sentinel_ && parent->right == node_) {
        // the order of conditions can't be swapped, or problem may occur when size_ == 1
        node_ = parent;
        parent = node_->parent;
      }
      node_ = parent;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --*this;
      return tmp;
    }
    const_iterator& operator--() {
      if(node_ == sentinel_->left) throw invalid_iterator(); // --cbegin() is invalid
      if(node_->left != nullptr) {
        node_ = node_->left;
        while(node_->right != nullptr) node_ = node_->right;
        return *this;
      }
      Node *parent = node_->parent;
      while(parent->left == node_) {
        node_ = parent;
        parent = node_->parent;
      }
      // parent shouldn't be sentinel, for *this can't be begin() here.
      node_ = parent;
      return *this;
    }
    value_type& operator*() const {
      return *node_->val_ptr;
    }
    value_type* operator->() const noexcept {
      return &**this;
    }
    bool operator==(const iterator &rhs) const {
      return container_ == rhs.container_ && node_ == rhs.node_;
    }
    bool operator==(const const_iterator &rhs) const {
      return container_ == rhs.container_ && node_ == rhs.node_;
    }
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };
  map(): size_(0), sentinel_(new Node) {
    sentinel_->left = sentinel_->right = sentinel_->parent = sentinel_;
  }
  map(const map &other): map() {
    *this = other;
  }
  map& operator=(const map &other) {
    clear();
    size_ = other.size_;
    if(other.empty()) return *this;
    Node *leftmost = nullptr, *rightmost = nullptr;
    auto copy_tree = [&leftmost, &rightmost, &copy_tree](Node *des, Node *src) -> void {
      if(src->left != nullptr) {
        leftmost = des->left = src->left->val_col_clone();
        des->left->parent = des;
        copy_tree(des->left, src->left);
      }
      if(src->right != nullptr) {
        rightmost = des->right = src->right->val_col_clone();
        des->right->parent = des;
        copy_tree(des->right, src->right);
      }
    };
    sentinel_->parent = other.sentinel_->parent->val_col_clone();
    sentinel_->parent->parent = sentinel_;
    copy_tree(sentinel_->parent, other.sentinel_->parent);
    sentinel_->left = leftmost;
    sentinel_->right = rightmost;
  }
  ~map() {
    clear();
    delete sentinel_;
  }
  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T& at(const Key &key) {
    if(empty()) throw index_out_of_bound();
    Node *cur = sentinel_->parent;
    while(cur != nullptr) {
      if(cur->val_ptr->first == key)
        return cur->val_ptr->second;
      if(comparer_(key, cur->val_ptr->first))
        cur = cur->left;
      else cur = cur->right;
    }
    throw index_out_of_bound();
  }
  const T& at(const Key &key) const {
    if(empty()) throw index_out_of_bound();
    Node *cur = sentinel_->parent;
    while(cur != nullptr) {
      if(cur->val_ptr->first == key)
        return cur->val_ptr->second;
      if(comparer_(key, cur->val_ptr->first))
        cur = cur->left;
      else cur = cur->right;
    }
    throw index_out_of_bound();
  }
  /**
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T& operator[](const Key &key) {
    static_assert(std::is_default_constructible<Key>::value,
      "Default constructor is required for value type if you want to use no-const-qualified operator[].");
    if(empty()) {
      sentinel_->parent = sentinel_->left = sentinel_->right
        = new Node(std::make_pair(key, T()), sentinel_, nullptr, nullptr, NodeColor::kRed);
      ++size_;
      return sentinel_->parent->val_ptr->second;
    }
    Node *cur = sentinel_->parent, *parent = sentinel_; // sentinel_->parent != nullptr
    int is_left = true;
    while(cur != nullptr) {
      if(key == cur->val_ptr->first)
        return cur->val_ptr->second;
      parent = cur;
      if(comparer_(key, cur->val_ptr->first)) {
        cur = cur->left;
        is_left = true;
      } else {
        cur = cur->right;
        is_left = false;
      }
    }
    ++size_;
    value_type value = std::make_pair(key, T());
    if(is_left) {
      parent->left = new Node(value, parent, nullptr, nullptr, NodeColor::kRed);
      if(comparer_(value, *sentinel_->left->val_ptr))
        sentinel_->left = parent->left;
      if(comparer_(value, *sentinel_->right->val_ptr))
        sentinel_->right = parent->left;
      Node *res = parent->left;
      maintain(parent->left);
      return res->val_ptr->second;
    } else {
      parent->right = new Node(value, parent, nullptr, nullptr, NodeColor::kRed);
      if(comparer_(value, *sentinel_->left->val_ptr))
        sentinel_->left = parent->right;
      if(comparer_(value, *sentinel_->right->val_ptr))
        sentinel_->right = parent->right;
      Node *res = parent->right;
      maintain(parent->right);
      return res->val_ptr->second;
    }
  }
  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T& operator[](const Key &key) const {
    return at(key);
  }
  iterator begin() {
    return {*this, sentinel_->left};
  }
  const_iterator cbegin() const {
    return {*this, sentinel_->left};
  }
  iterator end() {
    return {*this, sentinel_};
  }
  const_iterator cend() const {
    return {*this, sentinel_};
  }
  bool empty() const {
    return size_ == 0; // or "sentinel_->parent == sentinel_->left == sentinel_"
  }
  size_t size() const {
    return size_;
  }
  void clear() {
    if(empty()) return;
    auto clear_tree = [&clear_tree](Node *cur) -> void {
      if(cur->left != nullptr) clear_tree(cur->left);
      if(cur->right != nullptr) clear_tree(cur->right);
      delete cur;
    };
    clear(sentinel_->parent);
    sentinel_->parent = sentinel_->left = sentinel_->right = sentinel_;
    size_ = 0;
  }
  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    if(empty()) {
      ++size_;
      sentinel_->parent = sentinel_->left = sentinel_->right
        = new Node(value, sentinel_, nullptr, nullptr, NodeColor::kRed);
      return {{*this, sentinel_->parent}, true};
    }
    Node *cur = sentinel_->parent, *parent = sentinel_; // sentinel_->parent != nullptr
    int is_left = true;
    while(cur != nullptr) {
      if(value.first == cur->val_ptr->first)
        return {{*this, cur}, false};
      parent = cur;
      if(comparer_(value.first, cur->val_ptr->first)) {
        cur = cur->left;
        is_left = true;
      } else {
        cur = cur->right;
        is_left = false;
      }
    }
    ++size_;
    if(is_left) {
      parent->left = new Node(value, parent, nullptr, nullptr, NodeColor::kRed);
      if(comparer_(value, *sentinel_->left->val_ptr))
        sentinel_->left = parent->left;
      if(comparer_(value, *sentinel_->right->val_ptr))
        sentinel_->right = parent->left;
      iterator res = {*this, parent->left};
      insert_maintain(parent->left);
      return {res, true};
    } else {
      parent->right = new Node(value, parent, nullptr, nullptr, NodeColor::kRed);
      if(comparer_(value, *sentinel_->left->val_ptr))
        sentinel_->left = parent->right;
      if(comparer_(value, *sentinel_->right->val_ptr))
        sentinel_->right = parent->right;
      iterator res = {*this, parent->right};
      insert_maintain(parent->right);
      return {res, true};
    }
  }
  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if(pos.container_ != this || pos->node_ == sentinel_ || pos->node_->parent == nullptr)
      throw invalid_iterator();
    if(size_ == 1) {
      clear();
      return;
    }
    --size_;
    Node *node = pos->node_, *parent = node->parent;
    if(parent == sentinel_) {
      if(node->left == nullptr) {
        Node *replace_node = node->right;
        sentinel_->parent = replace_node;
        replace_node->parent = sentinel_;
        if(sentinel_->left == node) {
          sentinel_->left = replace_node;
          while(sentinel_->left->left != nullptr) sentinel_->left = sentinel_->left->left;
        }
        delete node;
        erase_maintain(replace_node);
      } else {
        Node *replace_node = node->left;
        sentinel_->parent = replace_node;
        replace_node->parent = sentinel_;
        if(sentinel_->right == node) {
          sentinel_->right = replace_node;
          while(sentinel_->right->right != nullptr) sentinel_->right = sentinel_->right->right;
        }
        delete node;
        erase_maintain(replace_node);
      }
      return;
    }
    if(node->left == nullptr && node->right == nullptr) {
      if(parent->left == node) {
        parent->left = nullptr;
        if(sentinel_->left == node) sentinel_->left = parent;
      } else {
        parent->right = nullptr;
        if(sentinel_->right == node) sentinel_->right = parent;
      }
      delete node;
      erase_maintain(parent);
      return;
    }
    if(node->left == nullptr) {
      Node *replace_node = node->right;
      if(parent->left == node) parent->left = replace_node;
      else parent->right = replace_node;
      replace_node->parent = parent;
      if(sentinel_->left == node) {
        sentinel_->left = replace_node;
        while(sentinel_->left->left != nullptr) sentinel_->left = sentinel_->left->left;
      }
      delete node;
      erase_maintain(replace_node);
    } else {
      Node *replace_node = node->left;
      if(parent->left == node) parent->left = replace_node;
      else parent->right = replace_node;
      replace_node->parent = parent;
      if(sentinel_->right == node) {
        sentinel_->right = replace_node;
        while(sentinel_->right->right != nullptr) sentinel_->right = sentinel_->right->right;
      }
      delete node;
      erase_maintain(replace_node);
    }
  }
  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    return find(key) == end() ? 0 : 1;
  }
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    if(empty()) return end();
    Node *cur = sentinel_->parent;
    while(cur != nullptr) {
      if(cur->val_ptr->first == key)
        return {*this, cur};
      if(comparer_(key, cur->val_ptr->first))
        cur = cur->left;
      else cur = cur->right;
    }
    return end();
  }
  const_iterator find(const Key &key) const {
    if(empty()) return cend();
    Node *cur = sentinel_->parent;
    while(cur != nullptr) {
      if(cur->val_ptr->first == key)
        return {*this, cur};
      if(comparer_(key, cur->val_ptr->first))
        cur = cur->left;
      else cur = cur->right;
    }
    return cend();
  }
};

}

#endif
