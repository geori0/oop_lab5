#pragma once

#include <list>

class CustomMemoryResource : public std::pmr::memory_resource {
public:
  struct MemoryBlock {
    void *ptr;
    size_t size;
    bool used;
  };

  std::list<MemoryBlock> used_blocks;

  void *do_allocate(size_t bytes, size_t alignment) override;

  void do_deallocate(void *ptr, size_t bytes, size_t alignment) override;

  bool
  do_is_equal(const std::pmr::memory_resource &other) const noexcept override;

  ~CustomMemoryResource() override;
};