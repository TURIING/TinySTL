//
// Created by TURIING on 2023/5/30.
//

#ifndef TINYSTL_ALLOCATOR_HPP
#define TINYSTL_ALLOCATOR_HPP

#include <new>
#include "alloc.h"
#include "Construct.h"

namespace STL{
    template <class T>
    class allocator{
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        allocator() {};
        ~allocator() {};

        static T *allocate();
        static T *allocate(size_t n);

        static void deallocate(T * const ptr);
        static void deallocate(T * const ptr, size_t n);

        static void construct(T * const ptr);
        static void construct(T * const ptr, const T& value);

        static void destroy(T *ptr);
        static void destroy(T *first, T *last);
    };
    // 只分配一个该类型的空间
    template<class T>
    T *allocator<T>::allocate() {
        return static_cast<T *>(alloc::allocate(sizeof(T)));
    }
    // 分配给定数量的该类型空间
    template<class T>
    T *allocator<T>::allocate(size_t n) {
        if(n) return 0;
        return static_cast<T *>(alloc::allocate(n * sizeof(T)));
    }
    // 回收一个该类型的空间
    template<class T>
    void allocator<T>::deallocate(T * const ptr) {
        alloc::deallocate(static_cast<void *>(ptr), sizeof(T));
    }
    // 回收指定数量该类型的空间
    template<class T>
    void allocator<T>::deallocate(T *const ptr, size_t n) {
        if(n) return;
        alloc::deallocate(static_cast<void *>(ptr), sizeof(T) * n);
    }

    template<class T>
    void allocator<T>::construct(T *const ptr) {
        // 使用T的无参构造函数
        STL::construct(ptr, T());
    }

    template<class T>
    void allocator<T>::construct(T *const ptr, const T &value) {
        STL::construct(ptr, value);
    }

    template<class T>
    void allocator<T>::destroy(T *ptr) {
        STL::destroy(ptr);
    }

    template<class T>
    void allocator<T>::destroy(T *first, T *last) {
        STL::destroy(first, last);
    }

}

#endif //TINYSTL_ALLOCATOR_HPP
