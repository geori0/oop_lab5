#include "custom_memory_resource.h"

#include <iostream>

void *CustomMemoryResource::do_allocate(size_t bytes, size_t alignment) {
  for (MemoryBlock &block : used_blocks) {
    if (!block.used && block.size >= bytes) {
      block.used = true;
      return block.ptr;
    }
  }
  void *ptr = ::operator new(bytes, std::align_val_t(alignment));
  used_blocks.push_back({ptr, bytes, true});

  return ptr;
}

void CustomMemoryResource::do_deallocate(void *ptr, size_t bytes,
                                         size_t alignment) {
  if (ptr == nullptr)
    return;
  for (MemoryBlock &block : used_blocks) {
    if (block.ptr == ptr) {
      if (!block.used) {
        throw std::logic_error("Попытка двойного освобождения памяти");
      }
      block.used = false;
      return;
    }
  }
  throw std::logic_error("Попытка освобождения не выделенного блока");
}

bool CustomMemoryResource::do_is_equal(
    const std::pmr::memory_resource &other) const noexcept {
  return this == &other;
}

CustomMemoryResource::~CustomMemoryResource() {
  for (MemoryBlock &block : used_blocks) {
    if (block.ptr != nullptr && block.used) {
      ::operator delete(block.ptr, std::align_val_t(alignof(std::max_align_t)));
      block.ptr = nullptr;
    }
  }
}
