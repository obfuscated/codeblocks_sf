// Experimental custom allocator for ProjectFile and Token classes


#ifndef __BLOCKALLOC_H__
#define __BLOCKALLOC_H__

#ifdef new
#undef new
#endif

#include <vector>
#include <list>

template <class T, int pool_size>
class BlockAllocator
{
    typedef std::list<T*> freelist;
    typedef std::vector<char*> blocks;

    blocks allocBlocks;
    freelist freeList;

    BlockAllocator()
    {
        AllocBlock();
    };

    ~BlockAllocator()
    {
        for(unsigned int i = 0; i < allocBlocks.size(); ++i)
            delete[] allocBlocks[i];
    };

    void AllocBlock()
    {
        char* ptr = new char[pool_size*sizeof(T)];
        allocBlocks.push_back(ptr);

        T* ptrT = (T*)(ptr);
        for(unsigned int i = 0; i < pool_size; ++i)
            freeList.push_back(&ptrT[i]);
    };

    static inline BlockAllocator* Get()
    {
        static BlockAllocator instance;
        return &instance;
    };

public:
    static inline T* New()
    {
        if(Get()->freeList.empty())
            Get()->AllocBlock();

        T *p = Get()->freeList.front();
        Get()->freeList.pop_front();
        return p;
    };

    static inline void Delete(T *ptr)
    {
        Get()->freeList.push_front(ptr);
    };
};



template <class T, int pool_size>
class BlockAllocated
{
public:
    inline void* operator new(size_t sizeofT)
    {
        assert(sizeofT == sizeof(T)); // this class is not inheritance-safe!
        return (void *) BlockAllocator<T, pool_size>::New();
    };

    inline void operator delete(void *ptr)
    {
        BlockAllocator<T, pool_size>::Delete(static_cast<T*>(ptr));
    };
};



#endif
