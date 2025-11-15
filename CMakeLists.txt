cmake_minimum_required(VERSION 3.10)
project(lab5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.0
  TLS_VERIFY false
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(main src/main.cpp)
target_include_directories(main PUBLIC include)


add_library(custom src/custom_memory_resource.cpp)
target_include_directories(custom PUBLIC include)


target_link_libraries(main PRIVATE custom)

enable_testing()

add_executable(tests test/tests.cpp)
target_include_directories(tests PUBLIC include)
target_link_libraries(tests PRIVATE custom gtest_main)

add_test(NAME MyProjectTests COMMAND tests)