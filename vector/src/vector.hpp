#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu 
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector 
{
private:
	T *data_;
	size_t size_, capacity_;
	std::allocator<T> allocator_;
	const bool is_move_constructable_ = std::is_move_constructible<T>::value;

public:
	/**
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator 
	{
		friend vector;

	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the 
	// iterator points to. 
	// STL algorithms and containers may use these type_traits (e.g. the following 
	// typedef) to work properly. In particular, without the following code, 
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;


	private:
		const vector *container_ptr_;
		size_t index_;
	public:
		iterator(const vector &container, const size_t &index)
			: container_ptr_(&container), index_(index) {}
		iterator(const vector *container_ptr, const size_t &index)
			: container_ptr_(container_ptr), index_(index) {}
		iterator(const iterator &other): container_ptr_(other.container_ptr_), index_(other.index_) {}
		// iterator(iterator &&other);
		iterator& operator=(const iterator &other) {
			if(this == &other) return *this;
			container_ptr_ = other.container_ptr_;
			index_ = other.index_;
			return *this;
		}
		// iterator& operator=(iterator &&other);
		iterator operator+(const difference_type &n) const {
			if(n < 0) return *this - (-n);
			if(index_ + n > container_ptr_->size_) throw invalid_iterator();
			return {container_ptr_, index_ + n};
		}
		iterator operator-(const difference_type &n) const {
			if(n < 0) return *this + (-n);
			if(index_ < n) throw index_out_of_bound();
			return {container_ptr_, index_ - n};
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		difference_type operator-(const iterator &rhs) const {
			if(container_ptr_ != rhs.container_ptr_) throw invalid_iterator();
			return static_cast<difference_type>(index_)- static_cast<difference_type>(rhs.index_);
		}
		iterator& operator+=(const difference_type &n) {
			if(n < 0) return *this -= (-n);
			if(index_ + n > container_ptr_->size_) throw invalid_iterator();
			index_ += n;
			return *this;
		}
		iterator& operator-=(const difference_type &n) {
			if(n < 0) return *this += (-n);
			if(index_ < n) throw invalid_iterator();
			index_ -= n;
			return *this;
		}
		iterator operator++(int) {
			iterator tmp = *this;
			*this += 1;
			return tmp;
		}
		iterator& operator++() {
			return *this += 1;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			*this -= 1;
			return tmp;
		}
		iterator& operator--() {
			return *this -= 1;
		}
		T& operator*() const {
			return container_ptr_->data_[index_];
		}
		T* operator->() const {
			return &(**this);
		}
		bool operator==(const iterator &rhs) const {
			return *this - rhs == 0;
		}
		bool operator==(const const_iterator &rhs) const {
			return container_ptr_ == rhs.container_ptr_ && index_ == rhs.index_;
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator 
	{
		friend vector;

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	
	private:
		const vector *container_ptr_;
		size_t index_;
	public:
		const_iterator(const vector &container, const size_t &index)
		  : container_ptr_(&container), index_(index) {}
		const_iterator(const vector *container_ptr, const size_t &index)
			: container_ptr_(container_ptr), index_(index) {}
		const_iterator(const const_iterator &other): container_ptr_(other.container_ptr_), index_(other.index_) {}
		// const_iterator(const_iterator &&other);
		const_iterator& operator=(const const_iterator &other) {
			if(this == &other) return *this;
			container_ptr_ = other.container_ptr_;
			index_ = other.index_;
			return *this;
		}
		// const_iterator& operator=(const_iterator &&other);
		const_iterator operator+(const difference_type &n) const {
			if(n < 0) return *this - (-n);
			if(index_ + n > container_ptr_->size_) throw invalid_iterator();
			return {container_ptr_, index_ + n};
		}
		const_iterator operator-(const difference_type &n) const {
			if(n < 0) return *this + (-n);
			if(index_ < n) throw invalid_iterator();
			return {container_ptr_, index_ - n};
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		difference_type operator-(const const_iterator &rhs) const {
			if(container_ptr_ != rhs.container_ptr_) throw invalid_iterator();
			return static_cast<difference_type>(index_)- static_cast<difference_type>(rhs.index_);
		}
		const_iterator& operator+=(const difference_type &n) {
			if(n < 0) return *this -= (-n);
			if(index_ + n > container_ptr_->size_) throw invalid_iterator();
			index_ += n;
			return *this;
		}
		const_iterator& operator-=(const difference_type &n) {
			if(n < 0) return *this += (-n);
			if(index_ < n) throw index_out_of_bound();
			index_ -= n;
			return *this;
		}
		const_iterator operator++(int) {
			iterator tmp = *this;
			*this += 1;
			return tmp;
		}
		const_iterator& operator++() {
			return *this += 1;
		}
		const_iterator operator--(int) {
			iterator tmp = *this;
			*this -= 1;
			return tmp;
		}
		const_iterator& operator--() {
			return *this -= 1;
		}
		const T& operator*() const {
			return container_ptr_->data_[index_];
		}
		const T* operator->() const {
			return &(**this);
		}
		bool operator==(const const_iterator &rhs) const {
			return *this - rhs == 0;
		}
		bool operator==(const iterator &rhs) const {
			return container_ptr_ == rhs.container_ptr_ && index_ == rhs.index_;
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}

	};
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector(): size_(0), capacity_(2048) {
		data_ = allocator_.allocate(capacity_);
	}
	vector(const vector &other) {
		size_ = other.size_;
		capacity_ = other.capacity_;
		data_ = allocator_.allocate(capacity_);
		for(size_t i = 0; i < size_; ++i)
			new (data_ + i) T(other.data_[i]);
	}
	vector(vector &&other) {
		size_ = other.size_;
		capacity_ = other.capacity_;
		data_ = other.data_;
		other.data_ = nullptr;
	}
	~vector() {
		clear();
		allocator_.deallocate(data_, capacity_);
		capacity_ = 0;
	}
	vector &operator=(const vector &other) {
		if(this == &other) return *this;
		clear();
		reserve(other.capacity_);
		size_ = other.size_;
		for(size_t i = 0; i < size_; ++i)
			new (data_ + i) T(other.data_[i]);
		return *this;
	}
	vector &operator=(vector &&other) {
		if(this == &other) return *this;
		size_ = other.size_;
		capacity_ = other.capacity_;
		data_ = other.data_;
		other.size_ = 0;
		other.capacity_ = 0;
		other.data_ = nullptr;
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T& at(const size_t &pos) {
		if(pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T& at(const size_t &pos) const {
		if(pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T& operator[](const size_t &pos) {
		if(pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T& operator[](const size_t &pos) const {
		if(pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		if(empty()) throw container_is_empty();
		return data_[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		if(empty()) throw container_is_empty();
		return data_[size_ - 1];
	}
	iterator begin() {
		return {*this, 0};
	}
	const_iterator cbegin() const {
		return {*this, 0};
	}
	iterator end() {
		return {*this, size_};
	}
	const_iterator cend() const {
		return {*this, size_};
	}
	bool empty() const {
		return size_ == 0;
	}
	size_t size() const {
		return size_;
	}
	void clear() {
		for(size_t i = 0; i < size_; ++i)
			data_[i].~T();
		size_ = 0;
	}
	void reserve(size_t capacity) {
		if(capacity_ >= capacity) return;
		T *new_data = allocator_.allocate(capacity);
		if(is_move_constructable_) {
			for(size_t i = 0; i < size_; ++i) {
				new (new_data + i) T(std::move(data_[i]));
				data_[i].~T();
			}
		} else {
			for(size_t i = 0; i < size_; ++i) {
				new (new_data + i) T(data_[i]);
				data_[i].~T();
			}
		}
		allocator_.deallocate(data_, capacity_);

		data_ = new_data;
		capacity_ = capacity;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		size_t index = pos.index_;
		if(index > size_) throw index_out_of_bound();
		if(size_ == capacity_) reserve(capacity_ << 1); // new capacity may be larger than size_t_max.
		++size_;
		if(is_move_constructable_) {
			for(size_t i = size_ - 1; i > index; --i) {
				new (data_ + i) T(std::move(data_[i - 1]));
				data_[i - 1].~T();
			}
		} else {
			for(size_t i = size_ - 1; i > index; --i) {
				new (data_ + i) T(data_[i - 1]);
				data_[i - 1].~T();
			}
		}
		new (data_ + index) T(value);
		return pos;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		return insert(iterator(this, ind), value);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		size_t index = pos.index_;
		if(index >= size_) throw index_out_of_bound();
		--size_;
		data_[index].~T();
		if(is_move_constructable_) {
			for(size_t i = index; i < size_; ++i) {
				new (data_ + i) T(std::move(data_[i + 1]));
				data_[i + 1].~T();
			}
		} else {
			for(size_t i = index; i < size_; ++i) {
				new (data_ + i) T(data_[i + 1]);
				data_[i + 1].~T();
			}
		}
		return pos;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
		return erase(iterator(this, ind));
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if(size_ == capacity_) reserve(capacity_ << 1); // new capacity may be larger than size_t_max
		new (data_ + size_) T(value);
		++size_;
	}
	void push_back(T &&value) {
		if(size_ == capacity_) reserve(capacity_ << 1);
		new (data_ + size_) T(std::move(value));
		++size_;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if(empty()) throw container_is_empty();
		--size_;
		data_[size_].~T();
	}
};


}

#endif
