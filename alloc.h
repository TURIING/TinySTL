//
// Created by TURIING on 2023/5/27.
//

#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <cstddef>
#include <cstdlib>

namespace STL {
    constexpr std::size_t ALIGN = 8;                            // freelist每个下标对应内存块递增的大小
    constexpr std::size_t MAX_BYTES = 128;                      // freelist最后一个下标对应内存块的大小
    constexpr std::size_t NFREELISTS = MAX_BYTES / ALIGN;       // freelist数组的size
    constexpr std::size_t NOBJS = 20;                           // freelist每个下标对应内存块数

    class alloc {
    public:
        alloc();
        ~alloc();
        static void *allocate(std::size_t bytes);               // 分配空间
        static void deallocate(void *ptr, std::size_t bytes);   // 回收已分配空间
        static void *reallocate(void *ptr, std::size_t old_sz, std::size_t new_sz); // 扩容已分配空间
    private:
        static std::size_t find_index(std::size_t bytes);                       // 通过字节数找到freelist对应下标
        static void *refill(std::size_t n);
        static char *chunk_alloc(std::size_t size, std::size_t &n_objs);
        static std::size_t round_up(std::size_t bytes);                         // 返回将给定的字节数向上取整（8的倍数）
    private:
        union Obj {                                             // 自由链表节点
            union Obj* freelist_link;
            char client_data[1];
        };
        static Obj *freelist[NFREELISTS];                       // 自由链表
        static char *start_free;                                // 内存池起始位置
        static char *end_free;                                  // 内存池终止位置
        static std::size_t heap_size;                           // 内存池已分配大小
    };

} // STL

#endif //TINYSTL_ALLOC_H
