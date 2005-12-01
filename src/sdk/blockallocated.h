/*
*          Block allocator template classes for use with:
*          Code::Blocks Studio, an open-source cross-platform IDE
*          Copyright (C) 2003  Yiannis An. Mandravellos
*
* Author:  Thomas Denk
*
* WARNING: These template classes are NOT thread-safe and NOT inheritance-safe and are certain to crash your application
*          in an evil, unpredictable, and hard to debug way if you use them incorrectly. The allocator was designed
*          to work with a limited, known subset of classes without any intent of being general or reusable (or being reused).
*          Allocated pools are not freed until the application terminates, even if all of the subnodes have been deleted.
*          This is intended behaviour, and obviously unsuitable for scenarios with huge peak allocations.
*          Only use these classes if you know EXACTLY what you are doing. I will not provide any kind of support.
*
*          Although the allocator is up to 200 times faster than standard operator new/delete, the possible gains
*          for most applications are neglegible, as most applications do not spend significant time in new/delete anyway.
*          Unless you use new/delete 10.000+ times per second, using the block allocator will buy you no noticeable benefit.
*
*          Standard operator new is a lot more conservative with heap space, it performs just fine in most cases, it does not
*          depend on proper parameter tuning, and it is guaranteed to work with any kind of class without any malign behaviour.
*
*          Defining DEBUG_BLOCKALLOC will collect performance data and enable leak detection.
*
* $Id$
* $Date$
*/

#ifndef __BLOCKALLOC_H__
#define __BLOCKALLOC_H__

#ifdef new
#undef new
#endif

#include <vector>
#include <wx/file.h>


template <class T, unsigned int pool_size>
class BlockAllocator
{
    template <class U>
    class LinkedBlock
    {
    public:
        LinkedBlock<U> *next;
        char data[sizeof(U)];
    };

    std::vector<LinkedBlock<T>*> allocBlocks;

    LinkedBlock<T> *first;
    LinkedBlock<T> *last;
    int ref_count;
    int max_refs;
    int total_refs;

    void AllocBlockPushBack()
    {
        LinkedBlock<T> *ptr = new LinkedBlock<T>[pool_size];

        allocBlocks.push_back(ptr);

        for(unsigned int i = 0; i < pool_size - 1; ++i)
            ptr[i].next = &ptr[i+1];

        ptr[pool_size - 1].next = 0;

        if(first == 0)
            first = ptr;

        if(last)
            last->next = ptr;

        last = &ptr[pool_size -1];
    };


    void PushFront(LinkedBlock<T> *p)
    {
        p->next = first;
        first = p;
    };

public:

    BlockAllocator() : first(0), last(0), ref_count(0), max_refs(0), total_refs(0)
    {}
    ;

    ~BlockAllocator()
    {
        #ifdef DEBUG_BLOCKALLOC
            wxString s;
            wxFile f(wxString("blockalloc_debug") << DEBUG_BLOCKALLOC << ".log", wxFile::write);
            s.Printf("%d reserved pools of size %d (%d total objects)\n"
            "Maximum number of allocated objects: %d\n"
            "Total number of allocations: %d\n"
            "Number of stale objects: %d %s",
            allocBlocks.size(), pool_size, allocBlocks.size() * pool_size,
            max_refs, total_refs, ref_count, (ref_count == 0 ? "" : "(smells of memory leak...)"));
            f.Write(s);
        #endif

        for(unsigned int i = 0; i < allocBlocks.size(); ++i)
            delete[] allocBlocks[i];
    };

    inline void* New()
    {
        #ifdef DEBUG_BLOCKALLOC
            ++ref_count;
            ++total_refs;
            max_refs = ref_count > max_refs ? ref_count : max_refs;
        #endif

        if(first == 0)
            AllocBlockPushBack();

        void *p = &(first->data);
        first = first->next;
        return p;
    };

    inline void Delete(void *ptr)
    {
        #ifdef DEBUG_BLOCKALLOC
            --ref_count;
		#endif

        PushFront((LinkedBlock<T> *) ((char *) ptr - sizeof(void*)));
    };
};


template <class T, unsigned int pool_size>
class BlockAllocated
{
    static BlockAllocator<T, pool_size> allocator;

public:

    inline void* operator new(size_t size)
    {
        return allocator.New();
    };

    inline void operator delete(void *ptr)
    {
        if(ptr == 0) // C++ standard requires this
            return;
        allocator.Delete(ptr);
    };
};
template<class T, unsigned int pool_size>
BlockAllocator<T, pool_size> BlockAllocated<T, pool_size>::allocator;



#endif
