#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
template <class Tp>
class vector {
public:
  class const_iterator;
  class iterator {
    friend vector<Tp>;
    friend const_iterator;

  public:
    using differnce_type = std::ptrdiff_t;
    using value_type = Tp;
    using pointer = Tp*;
    using reference = Tp&;
    using iterator_category = std::output_iterator_tag;

    iterator();
    iterator(const iterator &);
    iterator& operator=(const iterator &);
    iterator operator+(const differnce_type &) const;
    iterator operator-(const differnce_type &) const;
    // throw invalid_iterator if two containers are not same
    differnce_type operator-(const iterator &) const;
    iterator& operator+=(const differnce_type &);
    iterator& operator-=(const differnce_type &);
    iterator operator++(int);
    iterator& operator++();
    iterator operator--(int);
    iterator& operator--();
    Tp& operator*() const;
    Tp* operator->() const;
    bool operator==(const iterator &) const;
    bool operator==(const const_iterator &) const;
    bool operator!=(const iterator &) const;
    bool operator!=(const const_iterator &) const;

  private:
    const vector<Tp> *_container;
    size_t _index;
    iterator(const vector<Tp> *container, const size_t &index);
  };
  class const_iterator {
    friend vector<Tp>;
    friend iterator;

  public:
    using differnce_type = std::ptrdiff_t;
    using value_type = Tp;
    using pointer = Tp*;
    using reference = Tp&;
    using iterator_category = std::output_iterator_tag;

    const_iterator();
    const_iterator(const const_iterator &);
    const_iterator(const iterator &);
    const_iterator& operator=(const const_iterator &);
    const_iterator operator+(const differnce_type &) const;
    const_iterator operator-(const differnce_type &) const;
    // throw invalid_iterator if two containers are not same
    differnce_type operator-(const const_iterator &) const;
    const_iterator& operator+=(const differnce_type &);
    const_iterator& operator-=(const differnce_type &);
    const_iterator operator++(int);
    const_iterator& operator++();
    const_iterator operator--(int);
    const_iterator& operator--();
    const Tp& operator*() const;
    const Tp* operator->() const;
    bool operator==(const const_iterator &) const;
    bool operator==(const iterator &) const;
    bool operator!=(const const_iterator &) const;
    bool operator!=(const iterator &) const;

  private:
    const vector<Tp> *_container;
    size_t _index;
    const_iterator(const vector<Tp> *container, const size_t &index);
  };

  vector();
  vector(const vector &);
  vector(vector &&) noexcept;
  ~vector();
  vector &operator=(const vector &);
  vector &operator=(vector &&);
  // throw index_out_of_bound if pos is not in [0, size)
  Tp& at(const size_t &pos);
  // throw index_out_of_bound if pos is not in [0, size)
  const Tp& at(const size_t &pos) const;
  // throw index_out_of_bound if pos is not in [0, size)
  Tp& operator[](const size_t &pos);
  // throw index_out_of_bound if pos is not in [0, size)
  const Tp& operator[](const size_t &pos) const;
  // throw container_is_empty if size is 0
  const Tp& front() const;
  // throw container_is_empty if size is 0
  const Tp& back() const;
  iterator begin() const;
  iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  bool empty() const;
  size_t size() const;
  void clear();
  void reserve(const size_t &capacity);
  // throw invalid_iterator if iter is not valid
  // throw index_out_of_bound if iter has an index > size
  iterator insert(const iterator &iter, const Tp &value);
  // throw invalid_iterator if iter is not valid
  // throw index_out_of_bound if iter has an index > size
  iterator insert(const size_t &index, const Tp &value);
  // throw invalid_iterator if iter is not valid
  // throw index_out_of_bound if iter has an index >= size
  iterator erase(const iterator &iter);
  // throw invalid_iterator if iter is not valid
  // throw index_out_of_bound if iter has an index >= size
  iterator erase(const size_t &index);
  void push_back(const Tp &);
  void push_back(Tp &&);
  // throw container_is_empty if size == 0
  void pop_back();

private:
  Tp *_data;
  size_t _left, _right;
  size_t _capacity;
};

// vector::iterator

template <class Tp>
vector<Tp>::iterator::iterator()
  : _container(nullptr), _index(0) {}

template <class Tp>
vector<Tp>::iterator::iterator(const vector<Tp> *container, const size_t &index)
  : _container(container), _index(index) {}

template <class Tp>
vector<Tp>::iterator::iterator(const iterator &) = default;

template <class Tp>
typename vector<Tp>::iterator&
  vector<Tp>::iterator::operator=(const iterator &) = default;

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::iterator::operator+(const differnce_type &diff) const {
  if(diff < 0) return *this - (-diff);
  if(_index + diff > _container->size())
    throw index_out_of_bound{};
  return iterator{_container, _index + diff};
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::iterator::operator-(const differnce_type &diff) const {
  if(diff < 0) return *this + (-diff);
  if(_index < diff)
    throw index_out_of_bound{};
  return {_container, _index - diff};
}

template <class Tp>
typename vector<Tp>::iterator::differnce_type
  vector<Tp>::iterator::operator-(const iterator &other) const {
  if(_container != other._container)
    throw invalid_iterator{};
  return static_cast<differnce_type>(_index) - static_cast<differnce_type>(other._index);
}

template <class Tp>
typename vector<Tp>::iterator&
  vector<Tp>::iterator::operator+=(const differnce_type &diff) {
  if(diff < 0) return *this -= -diff;
  if(_index + diff > _container->size())
    throw index_out_of_bound{};
  _index += diff;
  return *this;
}

template <class Tp>
typename vector<Tp>::iterator&
  vector<Tp>::iterator::operator-=(const differnce_type &diff) {
  if(diff < 0) return *this += -diff;
  if(_index < diff)
    throw index_out_of_bound{};
  _index -= diff;
  return *this;
}

template <class Tp>
typename vector<Tp>::iterator&
  vector<Tp>::iterator::operator++() {
  return *this += 1;
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::iterator::operator++(int) {
  iterator tmp = *this;
  *this += 1;
  return tmp;
}

template <class Tp>
typename vector<Tp>::iterator&
  vector<Tp>::iterator::operator--() {
  return *this -= 1;
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::iterator::operator--(int) {
  iterator tmp = *this;
  *this -= 1;
  return tmp;
}

template <class Tp>
bool vector<Tp>::iterator::operator==(const iterator &other) const {
  return _container == other._container && _index == other._index;
}

template <class Tp>
bool vector<Tp>::iterator::operator==(const const_iterator &other) const {
  return _container == other._container && _index == other._index;
}

template <class Tp>
bool vector<Tp>::iterator::operator!=(const iterator &other) const {
  return _container != other._container || _index != other._index;
}

template <class Tp>
bool vector<Tp>::iterator::operator!=(const const_iterator &other) const {
  return _container != other._container || _index != other._index;
}

template <class Tp>
Tp& vector<Tp>::iterator::operator*() const {
  return _container->_data[_container->_left + _index];
}

template <class Tp>
Tp* vector<Tp>::iterator::operator->() const {
  return _container->_data + _container->_left + _index;
}



// vector::const_iterator

template <class Tp>
vector<Tp>::const_iterator::const_iterator()
  : _container(nullptr), _index(0) {}

template <class Tp>
vector<Tp>::const_iterator::const_iterator(const vector<Tp> *container, const size_t &index)
  : _container(container), _index(index) {}

template <class Tp>
vector<Tp>::const_iterator::const_iterator(const const_iterator &) = default;

template <class Tp>
vector<Tp>::const_iterator::const_iterator(const iterator &other)
  : _container(other._container), _index(other._index) {}

template <class Tp>
typename vector<Tp>::const_iterator&
  vector<Tp>::const_iterator::operator=(const const_iterator &) = default;

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::const_iterator::operator+(const differnce_type &diff) const {
  if(diff < 0) return *this - (-diff);
  if(_index + diff > _container->size())
    throw index_out_of_bound{};
  return {_container, _index + diff};
}

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::const_iterator::operator-(const differnce_type &diff) const {
  if(diff < 0) return *this + (-diff);
  if(_index < diff)
    throw index_out_of_bound{};
  return {_container, _index - diff};
}

template <class Tp>
typename vector<Tp>::const_iterator::differnce_type
  vector<Tp>::const_iterator::operator-(const const_iterator &other) const {
  if(_container != other._container)
    throw invalid_iterator{};
  return static_cast<differnce_type>(_index) - static_cast<differnce_type>(other._index);
}

template <class Tp>
typename vector<Tp>::const_iterator&
  vector<Tp>::const_iterator::operator+=(const differnce_type &diff) {
  if(diff < 0) return *this -= -diff;
  if(_index + diff > _container->size())
    throw index_out_of_bound{};
  _index += diff;
  return *this;
}

template <class Tp>
typename vector<Tp>::const_iterator&
  vector<Tp>::const_iterator::operator-=(const differnce_type &diff) {
  if(diff < 0) return *this += -diff;
  if(_index < diff)
    throw index_out_of_bound{};
  _index -= diff;
  return *this;
}

template <class Tp>
typename vector<Tp>::const_iterator&
  vector<Tp>::const_iterator::operator++() {
  return *this += 1;
}

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::const_iterator::operator++(int) {
  iterator tmp = *this;
  *this += 1;
  return tmp;
}

template <class Tp>
typename vector<Tp>::const_iterator&
  vector<Tp>::const_iterator::operator--() {
  return *this -= 1;
}

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::const_iterator::operator--(int) {
  iterator tmp = *this;
  *this -= 1;
  return tmp;
}

template <class Tp>
bool vector<Tp>::const_iterator::operator==(const const_iterator &other) const {
  return _container == other._container && _index == other._index;
}

template <class Tp>
bool vector<Tp>::const_iterator::operator==(const iterator &other) const {
  return _container == other._container && _index == other._index;
}

template <class Tp>
bool vector<Tp>::const_iterator::operator!=(const const_iterator &other) const {
  return _container != other._container || _index != other._index;
}

template <class Tp>
bool vector<Tp>::const_iterator::operator!=(const iterator &other) const {
  return _container != other._container || _index != other._index;
}

template <class Tp>
const Tp& vector<Tp>::const_iterator::operator*() const {
  return _container->_data[_container->_left + _index];
}

template <class Tp>
const Tp* vector<Tp>::const_iterator::operator->() const {
  return _container->_data + _container->_left + _index;
}




// vector

template <class Tp>
vector<Tp>::vector()
  : _data(nullptr), _left(0), _right(0), _capacity(0) {}

template <class Tp>
vector<Tp>::vector(const vector &other): vector() {
  if(other.empty()) return;
  _left = other._left;
  _right = other._right;
  _capacity = other._capacity;
  _data = static_cast<Tp*>(operator new(_capacity * sizeof(Tp)));
  for(size_t i = _left; i < _right; ++i)
    new(_data + i) Tp(other._data[i]);
}

template <class Tp>
vector<Tp>::vector(vector &&other) noexcept: vector() {
  if(other.empty()) return;
  _left = other._left;
  _right = other._right;
  _capacity = other._capacity;
  _data = other._data;
  other._left = 0;
  other._right = 0;
  other._capacity = 0;
  other._data = nullptr;
}

template <class Tp>
vector<Tp>::~vector() {
  clear();
  operator delete(_data);
  _data = nullptr;
  _left = 0;
  _right = 0;
  _capacity = 0;
}

template <class Tp>
vector<Tp>& vector<Tp>::operator=(const vector &other) {
  if(this == &other) return *this;
  clear();
  reserve(other._capacity);
  for(size_t i = _left; i < other._right; ++i)
    new(_data + i) Tp(other._data[i]);
  _left = other._left;
  _right = other._right;
  return *this;
}

template <class Tp>
vector<Tp>& vector<Tp>::operator=(vector &&other) {
  if(this == other) return *this;
  clear();
  operator delete(_data);
  _data = other._data;
  _left = other._left;
  _right = other._right;
  _capacity = other._capacity;
  other._data = nullptr;
  other._left = 0;
  other._right = 0;
  other._capacity = 0;
  return *this;
}

template <class Tp>
Tp& vector<Tp>::at(const size_t &pos) {
  if(pos >= size())
    throw index_out_of_bound{};
  return _data[_left + pos];
}

template <class Tp>
const Tp& vector<Tp>::at(const size_t &pos) const {
  if(pos >= size())
    throw index_out_of_bound{};
  return _data[_left + pos];
}

template <class Tp>
Tp& vector<Tp>::operator[](const size_t &pos) {
  if(pos >= size())
    throw index_out_of_bound{};
  return _data[_left + pos];
}

template <class Tp>
const Tp& vector<Tp>::operator[](const size_t &pos) const {
  if(pos >= size())
    throw index_out_of_bound{};
  return _data[_left + pos];
}

template <class Tp>
const Tp& vector<Tp>::front() const {
  if(empty())
    throw container_is_empty{};
  return _data[_left];
}

template <class Tp>
const Tp& vector<Tp>::back() const {
  if(empty())
    throw container_is_empty{};
  return _data[_right - 1];
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::begin() const {
  return iterator{this, 0};
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::end() const {
  return iterator{this, size()};
}

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::cbegin() const {
  return const_iterator{this, 0};
}

template <class Tp>
typename vector<Tp>::const_iterator
  vector<Tp>::cend() const {
  return const_iterator{this, size()};
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::insert(const iterator &iter, const Tp &value) {
  if(iter._container != this)
    throw invalid_iterator{};
  return insert(iter._index, value);
}

template <class Tp>
typename vector<Tp>::iterator
  vector<Tp>::insert(const size_t &index, const Tp &value) {
  if(index > size())
    throw index_out_of_bound{};
  if(index > size() / 2) {
    if(_right == _capacity)
      reserve((_capacity + 1) * 2);
    ++_right;
    for(size_t i = _right - 1; i > _left + index; --i)
      new (_data + i) Tp(std::move(_data[i - 1]));
  } else {
    if(_left == 0)
      reserve((_capacity + 1) * 2);
    --_left;
    for(size_t i = _left; i < _left + index; ++i)
      new(_data + i) Tp(std::move(_data[i + 1]));
  }
  new (_data + _left + index) Tp(value);
  return iterator{this, index};
}

template <class Tp>
typename vector<Tp>::iterator vector<Tp>::erase(const iterator &iter) {
  if(iter._container != this)
    throw invalid_iterator{};
  return erase(iter._index);
}

template <class Tp>
typename vector<Tp>::iterator vector<Tp>::erase(const size_t &index) {
  if(index >= size())
    throw index_out_of_bound{};
  _data[index].~Tp();
  if(index > size() / 2) {
    for(size_t i = _left + index; i < _right - 1; ++i)
      new (_data + i) Tp(std::move(_data[i + 1]));
    --_right;
  } else {
    for(size_t i = _left + index; i > _left; --i)
      new (_data + i) Tp(std::move(_data[i - 1]));
    ++_left;
  }
  return {this, index};
}

template <class Tp>
void vector<Tp>::push_back(const Tp &value) {
  if(_right == _capacity) reserve((_capacity + 1) * 2);
  new (_data + _right) Tp(value);
  ++_right;
}

template <class Tp>
void vector<Tp>::push_back(Tp &&value) {
  if(_right == _capacity) reserve((_capacity + 1) * 2);
  new (_data + _right) Tp(value);
  ++_right;
}

template <class Tp>
void vector<Tp>::pop_back() {
  if(empty())
    throw container_is_empty{};
  --_right;
  _data[_right].~Tp();
}

template <class Tp>
bool vector<Tp>::empty() const {
  return _right - _left == 0;
}

template <class Tp>
size_t vector<Tp>::size() const {
  return _right - _left;
}

template <class Tp>
void vector<Tp>::clear() {
  for(size_t i = _left; i < _right; ++i)
    _data[i].~Tp();
  _left = _right = _capacity / 2;
}

template <class Tp>
void vector<Tp>::reserve(const size_t &capacity) {
  if(_capacity >= capacity) return;
  Tp *new_data = static_cast<Tp*>(operator new(capacity * sizeof(Tp)));
  size_t old_size = size();
  size_t new_left = capacity / 2 - old_size / 2;
  for(size_t i = 0; i < old_size; ++i)
    new (new_data + new_left + i) Tp(std::move(_data[_left + i]));
  operator delete(_data);
  _left = new_left;
  _right = new_left + old_size;
  _data = new_data;
  _capacity = capacity;
}

}

#endif
