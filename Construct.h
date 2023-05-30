//
// Created by TURIING on 2023/5/30.
//

#ifndef TINYSTL_CONSTRUCT_H
#define TINYSTL_CONSTRUCT_H

#include "type_traits.h"
#include <new>

namespace STL {
    template<typename T1, typename T2>
    inline void construct(T1 *p, const T2 &value) {
        // placement new
        new(p) T1(value);
    }

    // 接受一个指针，将指针所指向的对象析构掉（直接调用对象的析构函数）
    template<typename T>
    inline void destroy(T *pointer) {
        pointer->~T();
    }
    // 接收两个迭代器，将[first，last]范围内的对象都析构掉
    template<class ForwordIterator>
    inline void destroy(ForwordIterator first, ForwordIterator last){
        __destroy(first, last);
    }

    template<class ForwardIterator>
    inline void __destroy(ForwardIterator first, ForwardIterator last) {
        // typename是必须的，它告诉编译器这是一个类型
        typedef typename STL::__type_traits<ForwardIterator>::has_trivial_destructor trivial_destructor;
        __destroy_aux(first, last, trivial_destructor());
    }

    template<class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, STL::__true_type) {}

    template<class ForwardIterator>
    void __destroy_aux(ForwardIterator first, ForwardIterator last, STL::__false_type) {
        for( ; first < last; ++first) {
            destroy(&*first);       // 改成destroy(first)也可以？？
        }
    }
}

#endif //TINYSTL_CONSTRUCT_H
