//
// Created by TURIING on 2023/5/27.
//

#include "alloc.h"

namespace STL {
    alloc::alloc() {}

    alloc::~alloc() {}

    alloc::Obj *alloc::freelist[NFREELISTS] = {nullptr};
    std::size_t alloc::heap_size = 0;

    void *alloc::allocate(std::size_t bytes) {
        // bytes大于分配空间上限
        if(bytes > MAX_BYTES){
            return malloc(bytes);
        }
        // 从freelist里取出内存块
        std::size_t index = find_index(bytes);
        auto node = freelist[index];
        // 如果freelist对应的下标没有内存块可分配，则向内存池请求分配内存，挂载到freelist对应的下标中
        if(!node)
            return refill(bytes);
        // 取下freelist其中一个下标下的内存块
        freelist[index] = node->freelist_link;
        return node;
    }

    void alloc::deallocate(void *ptr, std::size_t bytes) {
        // 欲回收的内存空间大于内存块大小的上限
        if(bytes > MAX_BYTES)
            free(ptr);
        // 回收内存空间至freelist中
        else {
            std::size_t index = find_index(bytes);
            Obj *node = static_cast<Obj *>(ptr);
            node->freelist_link = freelist[index];
            freelist[index] = node;
        }
    }

    void *alloc::reallocate(void *ptr, std::size_t old_sz, std::size_t new_sz) {
        deallocate(ptr, old_sz);
        return allocate(new_sz);
    }

    std::size_t alloc::find_index(std::size_t bytes) {
        return (bytes + ALIGN - 1)/ ALIGN -1;
    }
    /*
     * 处理从内存池申请来的内存块
     */
    void *alloc::refill(std::size_t bytes) {
        size_t n_objs = NOBJS;
        char *chunk = chunk_alloc(bytes, n_objs);
        Obj **node, *result, *current_obj, *next_obj;
        // 内存池只返回一块，则直接返回
        if(n_objs == 1)
            return chunk;
        // 如果返回了多块，则返回第一块，将剩余的内存块挂载到freelist相应下标中的链表
        node = freelist + find_index(bytes);
        result = (Obj *)chunk;
        *node = next_obj = (Obj *)(chunk + bytes);          // chunk+bytes意味着跳过了第一块（即要分配出去那块）
        for(std::size_t i = 1;; i++) {                      // 从1开始，是因为第0块已经分配出去了
            // 将所有内存块链接起来
            current_obj = next_obj;
            next_obj = (Obj *)((char *)next_obj + bytes);   // 先转成char *是为了将步长调整为1字节
            // 链接完了
            if(n_objs - 1 == i) {
                current_obj->freelist_link = nullptr;
                break;
            }else {
                current_obj->freelist_link = next_obj;
            }
        }
        return result;
    }
    /*
     * 从内存池中取出内存
     * size: 每块内存块的大小， n_objs: 传入参数，既是请求分配的块数也是最后反馈的分配块数
     */
    char *alloc::chunk_alloc(std::size_t size, size_t &n_objs) {
        char *result = nullptr;
        std::size_t byte_left = end_free - start_free;
        std::size_t need_bytes = size * n_objs;
        //内存池剩余空间完全满足需要，则直接返回总共need_bytes大小的内存块
        if(byte_left >= need_bytes) {
            result = start_free;
            start_free += need_bytes;
            return result;
        }
        //内存池剩余空间不能完全满足需要，但足够供应一个或以上的所需求大小区块，则返回最大能提供的那几个内存块
        else if (byte_left > size) {
            n_objs = byte_left / size;
            result = start_free;
            start_free += n_objs * size;
            return result;
        }
        //内存池剩余空间连一个区块的大小都无法提供，那就扩容内存池
        else {
            // 如果重新扩容前的内存池里还有剩余的内存空间，那就先分配给freelist
            if(byte_left > 0) {
                Obj **node = freelist + find_index(byte_left);
                ((Obj *)start_free)->freelist_link = *node;
                *node = (Obj *)start_free;
            }
            // 打算在原来内存池已申请的内存大小的基础上多申请两倍所需内存
            // heap_size代表之前内存池已申请的大小，右移4位，相当于除以16，是为了内存对齐
            std::size_t bytes_to_get = 2 * need_bytes + round_up(heap_size >> 4);
            start_free = (char *) malloc(bytes_to_get);
            // malloc都找不到空间，在bytes对应freelist的索引中，往上找更大的内存块，取下一块内存块，
            // 放入内存池中重新拆成小的内存块分配给freelist，其中内存碎片零头也被拆成更小的内存块放入freelist
            if(!start_free) {
                Obj **node = nullptr, *p = nullptr;
                for(int i = size; i <= MAX_BYTES; i += ALIGN){
                    node = freelist + find_index(i);
                    p = *node;
                    // p!=0,代表该下标存在
                    if(p) {
                        *node = p->freelist_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, n_objs);
                    }
                }
                end_free = nullptr;
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, n_objs);
        }
    }

    std::size_t alloc::round_up(std::size_t bytes) {
        return (bytes + ALIGN - 1) &~ (ALIGN - 1);
    }
} // STL