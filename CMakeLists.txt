cmake_minimum_required(VERSION 3.25)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)

add_executable(TinySTL main.cpp alloc.cpp alloc.h Allocator.hpp Construct.h type_traits.h)
