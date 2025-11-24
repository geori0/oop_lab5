#include <iostream>

#include "custom_memory_resource.h"
#include "vector.h"

int main() {

  CustomMemoryResource resource;
  std::pmr::polymorphic_allocator<int> alloc(&resource);
  Vector<int> vec(alloc);

  std::cout << "Добавляем элементы: " << std::endl;
  for (int i = 1; i <= 5; i++) {
    vec.PushItem(i * 10);
    std::cout << vec[i - 1] << " " << std::endl;
  }

  std::cout << "Итерация: " << std::endl;
  for (const auto &item : vec) {
    std::cout << item << " " << std::endl;
  }

  std::cout << "Размер: " << vec.getSize() << std::endl;
  std::cout << "vec[2] = " << vec[2] << std::endl;

  vec.PopItem();
  std::cout << "После PopItem: " << std::endl;
  for (const auto &item : vec) {
    std::cout << item << " " << std::endl;
  }

  return 0;
}