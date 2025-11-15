#pragma once

#include <iterator>

template <class T, class allocator_type = std::pmr::polymorphic_allocator<T>>
class Vector {
public:
  class Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    Iterator(pointer ptr);

    reference operator*() const;
    pointer operator->();

    Iterator &operator++();
    Iterator operator++(int);

    friend bool operator==(const Iterator &a, const Iterator &b) {
      return a.it_ptr == b.it_ptr;
    };
    friend bool operator!=(const Iterator &a, const Iterator &b) {
      return a.it_ptr != b.it_ptr;
    };

  private:
    pointer it_ptr;
  };

  Vector(allocator_type alloc = {});

  Vector(const Vector &other);
  Vector(Vector &&other) noexcept;
  ~Vector() noexcept;
  Vector &operator=(const Vector &other);
  Vector &operator=(Vector &&other) noexcept;
  void Resize(size_t new_size);
  T &operator[](size_t i);
  const T &operator[](size_t i) const;
  void PushItem(const T &t);
  void PushItem(T &&t);
  void PopItem();
  size_t getSize() const;
  Iterator begin();
  Iterator end();

private:
  size_t size_vector;
  T *data_vector;
  size_t capacity;
  allocator_type polymorphic_allocator;
};

#include "vector.ipp"