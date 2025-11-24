#include <memory_resource>

template <class T, class allocator_type>
Vector<T, allocator_type>::Vector(allocator_type alloc)
    : size_vector(0), capacity(2), polymorphic_allocator(alloc) {
  data_vector = polymorphic_allocator.allocate(capacity);
}

template <class T, class allocator_type>
Vector<T, allocator_type>::Vector(const Vector &other)
    : size_vector(other.size_vector), capacity(other.capacity),
      polymorphic_allocator(other.polymorphic_allocator) {
  data_vector = polymorphic_allocator.allocate(capacity);
  for (size_t i = 0; i < size_vector; ++i) {
    std::allocator_traits<allocator_type>::construct(
        polymorphic_allocator, data_vector + i, other.data_vector[i]);
  }
}

template <class T, class allocator_type>
Vector<T, allocator_type>::Vector(Vector &&other) noexcept
    : size_vector(other.size_vector), capacity(other.capacity),
      polymorphic_allocator(std::move(other.polymorphic_allocator)) {
  data_vector = other.data_vector;
  other.data_vector = nullptr;
  other.size_vector = 0;
  other.capacity = 0;
}

template <class T, class allocator_type>
size_t Vector<T, allocator_type>::getSize() const {
  return size_vector;
}

template <class T, class allocator_type>
void Vector<T, allocator_type>::Resize(size_t new_size) {
  T *new_data = polymorphic_allocator.allocate(new_size);
  for (size_t i = 0; i < size_vector; ++i) {
    std::allocator_traits<allocator_type>::construct(
        polymorphic_allocator, new_data + i, std::move(data_vector[i]));
    std::allocator_traits<allocator_type>::destroy(polymorphic_allocator,
                                                   data_vector + i);
  }
  polymorphic_allocator.deallocate(data_vector, capacity);
  data_vector = new_data;
  capacity = new_size;
}

template <class T, class allocator_type>
T &Vector<T, allocator_type>::operator[](size_t i) {
  if (i >= size_vector) {
    throw std::out_of_range("Ошибка index out of range");
  }
  return data_vector[i];
}

template <class T, class allocator_type>
const T &Vector<T, allocator_type>::operator[](size_t i) const {
  if (i >= size_vector) {
    throw std::out_of_range("Ошибка index out of range");
  }
  return data_vector[i];
}

template <class T, class allocator_type>
void Vector<T, allocator_type>::PushItem(const T &t) {
  if (capacity == size_vector) {
    Resize(capacity * 2);
  }
  std::allocator_traits<allocator_type>::construct(
      polymorphic_allocator, data_vector + size_vector, t);
  ++size_vector;
}

template <class T, class allocator_type>
void Vector<T, allocator_type>::PushItem(T &&t) {
  if (capacity == size_vector) {
    Resize(capacity * 2);
  }
  std::allocator_traits<allocator_type>::construct(
      polymorphic_allocator, data_vector + size_vector, std::move(t));
  ++size_vector;
}

template <class T, class allocator_type>
void Vector<T, allocator_type>::PopItem() {
  if (size_vector == 0) {
    throw std::out_of_range("Ошибка Cannot pop from empty vector");
  }
  --size_vector;
  std::allocator_traits<allocator_type>::destroy(polymorphic_allocator,
                                                 data_vector + size_vector);
}

template <class T, class allocator_type>
Vector<T, allocator_type> &
Vector<T, allocator_type>::operator=(const Vector &other) {
  if (this != &other) {
    for (size_t i = 0; i < size_vector; ++i) {
      std::allocator_traits<allocator_type>::destroy(polymorphic_allocator,
                                                     data_vector + i);
    }
    polymorphic_allocator.deallocate(data_vector, capacity);

    size_vector = other.size_vector;
    capacity = other.capacity;
    polymorphic_allocator = other.polymorphic_allocator;

    data_vector = polymorphic_allocator.allocate(capacity);
    for (size_t i = 0; i < size_vector; ++i) {
      std::allocator_traits<allocator_type>::construct(
          polymorphic_allocator, data_vector + i, other.data_vector[i]);
    }
  }
  return *this;
}

template <class T, class allocator_type>
Vector<T, allocator_type> &
Vector<T, allocator_type>::operator=(Vector &&other) noexcept {
  if (this != &other) {
    for (size_t i = 0; i < size_vector; ++i) {
      std::allocator_traits<allocator_type>::destroy(polymorphic_allocator,
                                                     data_vector + i);
    }
    polymorphic_allocator.deallocate(data_vector, capacity);

    size_vector = other.size_vector;
    capacity = other.capacity;
    data_vector = other.data_vector;
    polymorphic_allocator = std::move(other.polymorphic_allocator);

    other.data_vector = nullptr;
    other.size_vector = 0;
    other.capacity = 0;
  }
  return *this;
}

template <class T, class allocator_type>
Vector<T, allocator_type>::~Vector() noexcept {
  for (size_t i = 0; i < size_vector; ++i) {
    std::allocator_traits<allocator_type>::destroy(polymorphic_allocator,
                                                   data_vector + i);
  }
  polymorphic_allocator.deallocate(data_vector, capacity);
}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator
Vector<T, allocator_type>::begin() {
  return Iterator(data_vector);
}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator Vector<T, allocator_type>::end() {
  return Iterator(data_vector + size_vector);
}

template <class T, class allocator_type>
Vector<T, allocator_type>::Iterator::Iterator(pointer ptr) : it_ptr(ptr) {}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator::reference
Vector<T, allocator_type>::Iterator::operator*() const {
  return *it_ptr;
}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator::pointer
Vector<T, allocator_type>::Iterator::operator->() {
  return it_ptr;
}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator &
Vector<T, allocator_type>::Iterator::operator++() {
  it_ptr++;
  return *this;
}

template <class T, class allocator_type>
typename Vector<T, allocator_type>::Iterator
Vector<T, allocator_type>::Iterator::operator++(int) {
  Iterator tmp = *this;
  ++(*this);
  return tmp;
}