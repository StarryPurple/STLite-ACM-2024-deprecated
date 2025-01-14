#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
  struct Node {
    T *val_ptr = nullptr;
    Node *child = nullptr, *sibling = nullptr;
  };
  Node *root_;
  size_t size_;
  Compare comparer_;

  void copy_heap(Node *des, Node *src) {
    if(src->child == nullptr) return;
    des->child = new Node;
    des->child->val_ptr = new T(*src->child->val_ptr);
    copy_heap(des->child, src->child);
    Node *cur_src = src->child, *cur_des = des->child;
    while(cur_src->sibling != nullptr) {
      cur_des->sibling = new Node;
      cur_des->sibling->val_ptr = new T(*cur_src->sibling->val_ptr);
      copy_heap(cur_des->sibling, cur_src->sibling);
      cur_src = cur_src->sibling;
      cur_des = cur_des->sibling;
    }
  }
  void free_heap(Node *ptr) {
    if(ptr == nullptr) return;
    Node *cur = ptr->child, *del;
    delete ptr->val_ptr;
    delete ptr;
    while(cur != nullptr) {
      del = cur; cur = cur->sibling;
      free_heap(del);
    }
  }
  Node* multiple_merge(Node *cur) {
    if(cur == nullptr || cur->sibling == nullptr) return cur;
    Node *nxt = cur->sibling, *rest = multiple_merge(nxt->sibling);
    cur->sibling = nullptr;
    nxt->sibling = nullptr;
    if(comparer_(*cur->val_ptr, *nxt->val_ptr)) {
      cur->sibling = nxt->child;
      nxt->child = cur;
      cur = nxt;
    } else {
      nxt->sibling = cur->child;
      cur->child = nxt;
    }
    if(rest == nullptr) return cur;
    if(comparer_(*cur->val_ptr, *rest->val_ptr)) {
      cur->sibling = rest->child;
      rest->child = cur;
      return rest;
    } else {
      rest->sibling = cur->child;
      cur->child = rest;
      return cur;
    }
  }

public:
  priority_queue(): size_(0), root_(nullptr) {}
  priority_queue(const priority_queue &other): size_(other.size_) {
    root_ = new Node;
    root_->val_ptr = new T(*other.root_->val_ptr);
    copy_heap(root_, other.root_);
  }
  priority_queue(priority_queue &&other): root_(other.root_), size_(other.size_) {
    other.root_ = nullptr;
    other.size_ = 0;
  }
  ~priority_queue() {
    clear();
  }
  priority_queue &operator=(const priority_queue &other) {
    if(this == &other) return *this;
    clear();
    size_ = other.size_;
    root_ = new Node;
    root_->val_ptr = new T(*other.root_->val_ptr);
    copy_heap(root_, other.root_);
    return *this;
  }
  priority_queue &operator=(priority_queue &&other) {
    if(this == &other) return *this;
    clear();
    size_ = other.size_;
    root_ = other.root_;
    other.root_ = nullptr;
    other.size_ = 0;
    return *this;
  }
  const T& top() const {
    if(empty()) throw container_is_empty();
    return *root_->val_ptr;
  }
  void push(const T &e) {
    if(empty()) {
      size_ = 1;
      Node *node_ptr = new Node;
      node_ptr->val_ptr = new T(e);
      root_ = node_ptr;
      return;
    }
    ++size_;
    if(comparer_(*root_->val_ptr, e)) {
      Node *node_ptr = new Node;
      node_ptr->val_ptr = new T(e);
      node_ptr->child = root_;
      root_ = node_ptr;
      return;
    }
    Node *node_ptr = new Node;
    node_ptr->val_ptr = new T(e);
    node_ptr->sibling = root_->child;
    root_->child = node_ptr;
  }
  void pop() {
    if(empty()) throw container_is_empty();
    if(size_ == 1) {
      clear();
      return;
    }
    Node *cur = root_->child;
    delete root_->val_ptr;
    delete root_;
    root_ = multiple_merge(cur);
    --size_;
  }
  void clear() {
    free_heap(root_);
    root_ = nullptr;
    size_ = 0;
  }
  size_t size() const {
    return size_;
  }
  bool empty() const {
    return size_ == 0;
  }
  /**
   * merge two priority_queues with at most O(logn) complexity.
   * clear the other priority_queue.
   */
  void merge(priority_queue &other) {
    if(this == &other) {
      priority_queue another = *this;
      merge(another);
      return;
    }
    if(other.root_ == nullptr) return;
    if(root_ == nullptr) {
      *this = std::move(other);
      return;
    }
    if(comparer_(*root_->val_ptr, *other.root_->val_ptr)) {
      root_->sibling = other.root_->child;
      other.root_->child = root_;
      root_ = nullptr;
      other.size_ += size_;
      size_ = 0;
      *this = std::move(other);
      return;
    }
    other.root_->sibling = root_->child;
    root_->child = other.root_;
    other.root_ = nullptr;
    size_ += other.size_;
    other.size_ = 0;
  }
};

}

#endif
