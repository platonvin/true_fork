module;

/*
array with built-in index_counter that increments on move() and wraps around size
*/

import std;
#include <cassert>

export module ring;

export template <typename _Type>
class ring {
  public:
    ring(int initialSize) {
        assert(initialSize > 0);
        buffer = new _Type[initialSize];
        size_ = initialSize;
        current_index = 0;
    }
    ring(const ring &other) {
        size_ = other.size_;
        current_index = other.current_index;
        buffer = new _Type[size_];
        std::copy(other.buffer, other.buffer + size_, buffer);
    }
    ring() : buffer(nullptr), size_(0), current_index(0) {}

    ring(std::initializer_list<_Type> initList)
        : buffer(new _Type[initList.size()]), size_(initList.size()), current_index(0) {
        std::copy(initList.begin(), initList.end(), buffer);
    }

    ~ring() {
        delete[] buffer;
    }

    _Type *data() {
        return buffer;
    }

    void allocate(int size) {
        assert(size > 0);
        if (buffer) {
            delete[] buffer;
        }
        buffer = new _Type[size];
        size_ = size;
        current_index = 0;
    }

    // basically for changing FIF count
    void realloc(int newSize) {
        assert(newSize > 0);
        if (newSize == size_)
            return;

        _Type *newBuffer = new _Type[newSize];
        int elementsToCopy = (newSize < size_) ? newSize : size_;

        for (int i = 0; i < elementsToCopy; ++i) {
            newBuffer[i] = buffer[i];
        }

        delete[] buffer;
        buffer = newBuffer;
        size_ = newSize;
        if (current_index >= size_) {
            current_index = 0;
        }
    }

    _Type &next() {
        int nextIndex = (current_index + 1) % size_;
        return buffer[nextIndex];
    }
    _Type &previous() {
        int previousIndex = ((size_ + current_index) - 1) % size_;
        return buffer[previousIndex];
    }
    _Type &current() {
        return buffer[current_index];
    }

    void reset() {
        current_index = 0;
    }
    void move() {
        assert(size_ > 0);
        current_index = (current_index + 1) % size_;
    }

    int size() {
        return size_;
    }

    bool empty() {
        return size_ == 0;
    }

    ring &operator=(const ring &other) {
        if (this != &other) {
            delete[] buffer;
            size_ = other.size_;
            current_index = other.current_index;
            buffer = new _Type[size_];
            std::copy(other.buffer, other.buffer + size_, buffer);
        }
        return *this;
    }
    ring(ring &&other) {
        buffer = other.buffer;
        size_ = other.size_;
        current_index = other.current_index;
        other.buffer = nullptr;
        other.size_ = 0;
        other.current_index = 0;
    }
    ring &operator=(ring &&other) {
        if (this != &other) {
            delete[] buffer;
            buffer = other.buffer;
            size_ = other.size_;
            current_index = other.current_index;
            other.buffer = nullptr;
            other.size_ = 0;
            other.current_index = 0;
        }
        return *this;
    }
    _Type &operator[](int index) {
        assert(index < size_);
        return buffer[index];
    }

    class iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = _Type;
        using pointer = _Type *;
        using reference = _Type &;

        iterator(pointer ptr) : ptr(ptr) {}

        reference operator*() const {
            return *ptr;
        }
        pointer operator->() {
            return ptr;
        }
        iterator &operator++() {
            ptr++;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator &a, const iterator &b) {
            return a.ptr == b.ptr;
        };
        friend bool operator!=(const iterator &a, const iterator &b) {
            return a.ptr != b.ptr;
        };

      private:
        pointer ptr;
    };

    iterator begin() {
        return iterator(&buffer[0]);
    }

    iterator end() {
        return iterator(&buffer[size_]); // not size_ -1
    }

  private:
    _Type *buffer = nullptr;
    int size_ = 0;
    int current_index = 0;
};