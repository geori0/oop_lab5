#include <gtest/gtest.h>

#include <memory_resource>

#include "custom_memory_resource.h"
#include "vector.h"

class VectorIntTest : public ::testing::Test {
protected:
  void SetUp() override {
    resource = std::make_unique<CustomMemoryResource>();
    allocator =
        std::make_unique<std::pmr::polymorphic_allocator<int>>(resource.get());
  }

  void TearDown() override {
    allocator.reset();
    resource.reset();
  }

  std::unique_ptr<CustomMemoryResource> resource;
  std::unique_ptr<std::pmr::polymorphic_allocator<int>> allocator;
};

struct ComplexType {
  int id;
  double value;
  std::string name;

  ComplexType(int i = 0, double v = 0.0, const std::string &n = "")
      : id(i), value(v), name(n) {}

  bool operator==(const ComplexType &other) const {
    return id == other.id && value == other.value && name == other.name;
  }
};

class VectorComplexTypeTest : public ::testing::Test {
protected:
  void SetUp() override {
    resource = std::make_unique<CustomMemoryResource>();
    allocator = std::make_unique<std::pmr::polymorphic_allocator<ComplexType>>(
        resource.get());
  }

  void TearDown() override {
    allocator.reset();
    resource.reset();
  }

  std::unique_ptr<CustomMemoryResource> resource;
  std::unique_ptr<std::pmr::polymorphic_allocator<ComplexType>> allocator;
};

TEST_F(VectorIntTest, DefaultConstructor) {
  Vector<int> vec(*allocator);
  EXPECT_EQ(vec.getSize(), 0);
}

TEST_F(VectorIntTest, PushAndAccess) {
  Vector<int> vec(*allocator);

  vec.PushItem(1);
  vec.PushItem(2);
  vec.PushItem(3);

  EXPECT_EQ(vec.getSize(), 3);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
}

TEST_F(VectorIntTest, CopyConstructor) {
  Vector<int> vec1(*allocator);
  vec1.PushItem(1);
  vec1.PushItem(2);

  Vector<int> vec2(vec1);

  EXPECT_EQ(vec2.getSize(), 2);
  EXPECT_EQ(vec2[0], 1);
  EXPECT_EQ(vec2[1], 2);
}

TEST_F(VectorIntTest, MoveConstructor) {
  Vector<int> vec1(*allocator);
  vec1.PushItem(1);
  vec1.PushItem(2);

  Vector<int> vec2(std::move(vec1));

  EXPECT_EQ(vec2.getSize(), 2);
  EXPECT_EQ(vec2[0], 1);
  EXPECT_EQ(vec2[1], 2);
  EXPECT_EQ(vec1.getSize(), 0);
}

TEST_F(VectorIntTest, PopItem) {
  Vector<int> vec(*allocator);
  vec.PushItem(1);
  vec.PushItem(2);
  vec.PushItem(3);

  vec.PopItem();
  EXPECT_EQ(vec.getSize(), 2);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);

  vec.PopItem();
  EXPECT_EQ(vec.getSize(), 1);
  EXPECT_EQ(vec[0], 1);
}

TEST_F(VectorIntTest, PopFromEmpty) {
  Vector<int> vec(*allocator);
  EXPECT_THROW(vec.PopItem(), std::out_of_range);
}

TEST_F(VectorIntTest, OutOfRangeAccess) {
  Vector<int> vec(*allocator);
  vec.PushItem(1);

  EXPECT_THROW(vec[1], std::out_of_range);
  EXPECT_THROW(vec[100], std::out_of_range);
}

TEST_F(VectorIntTest, Resize) {
  Vector<int> vec(*allocator);

  vec.PushItem(1);
  vec.PushItem(2);

  vec.PushItem(3);

  EXPECT_EQ(vec.getSize(), 3);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
}

TEST_F(VectorIntTest, Iterator) {
  Vector<int> vec(*allocator);
  vec.PushItem(1);
  vec.PushItem(2);
  vec.PushItem(3);

  int sum = 0;
  int count = 0;
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    sum += *it;
    count++;
  }

  EXPECT_EQ(sum, 6);
  EXPECT_EQ(count, 3);
}

TEST_F(VectorIntTest, RangeBasedFor) {
  Vector<int> vec(*allocator);
  vec.PushItem(1);
  vec.PushItem(2);
  vec.PushItem(3);

  int sum = 0;
  for (const auto &item : vec) {
    sum += item;
  }

  EXPECT_EQ(sum, 6);
}

TEST_F(VectorComplexTypeTest, ComplexTypePushAndAccess) {
  Vector<ComplexType> vec(*allocator);

  vec.PushItem(ComplexType(1, 1.1, "first"));
  vec.PushItem(ComplexType(2, 2.2, "second"));

  EXPECT_EQ(vec.getSize(), 2);
  EXPECT_EQ(vec[0], ComplexType(1, 1.1, "first"));
  EXPECT_EQ(vec[1], ComplexType(2, 2.2, "second"));
}

TEST_F(VectorComplexTypeTest, ComplexTypeMoveSemantics) {
  Vector<ComplexType> vec(*allocator);

  ComplexType obj(1, 1.1, "test");
  vec.PushItem(std::move(obj));

  EXPECT_EQ(vec.getSize(), 1);
  EXPECT_EQ(vec[0], ComplexType(1, 1.1, "test"));
}

TEST_F(VectorComplexTypeTest, ComplexTypeIterator) {
  Vector<ComplexType> vec(*allocator);

  vec.PushItem(ComplexType(1, 1.1, "first"));
  vec.PushItem(ComplexType(2, 2.2, "second"));
  vec.PushItem(ComplexType(3, 3.3, "third"));

  int count = 0;
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    EXPECT_EQ(it->id, count + 1);
    count++;
  }

  EXPECT_EQ(count, 3);
}

TEST(CustomMemoryResourceTest, BasicAllocation) {
  CustomMemoryResource resource;
  std::pmr::polymorphic_allocator<int> allocator(&resource);

  Vector<int, std::pmr::polymorphic_allocator<int>> vec(allocator);

  vec.PushItem(1);
  vec.PushItem(2);
  vec.PushItem(3);

  EXPECT_EQ(vec.getSize(), 3);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);
}

TEST(CustomMemoryResourceTest, MemoryReuse) {
  CustomMemoryResource resource;
  std::pmr::polymorphic_allocator<int> allocator(&resource);

  {
    Vector<int, std::pmr::polymorphic_allocator<int>> vec(allocator);
    vec.PushItem(1);
    vec.PushItem(2);
  }

  {
    Vector<int, std::pmr::polymorphic_allocator<int>> vec(allocator);
    vec.PushItem(3);
    vec.PushItem(4);

    EXPECT_EQ(vec.getSize(), 2);
    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[1], 4);
  }
}

TEST(IntegrationTest, ComplexTypeWithCustomMemory) {
  CustomMemoryResource resource;
  std::pmr::polymorphic_allocator<ComplexType> allocator(&resource);

  Vector<ComplexType, std::pmr::polymorphic_allocator<ComplexType>> vec(
      allocator);

  for (int i = 0; i < 10; ++i) {
    vec.PushItem(ComplexType(i, i * 1.1, "Object_" + std::to_string(i)));
  }

  EXPECT_EQ(vec.getSize(), 10);

  int expected_id = 0;
  for (const auto &item : vec) {
    EXPECT_EQ(item.id, expected_id);
    EXPECT_EQ(item.value, expected_id * 1.1);
    expected_id++;
  }

  for (int i = 0; i < 5; ++i) {
    vec.PopItem();
  }
  EXPECT_EQ(vec.getSize(), 5);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}