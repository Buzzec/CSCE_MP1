/* 
    File: my_allocator.cpp

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the class MyAllocator.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "my_allocator.hpp"
#include <cassert>
#include <iostream>

/*--------------------------------------------------------------------------*/
/* NAME SPACES */
/*--------------------------------------------------------------------------*/

//using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS MyAllocator */
/*--------------------------------------------------------------------------*/

template<typename T>
T normalize(T block_size, T length){
    if(length % block_size == 0){
        return length;
    }
    else{
        return length - length % block_size + block_size;
    }
}

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) :
        block_size(_basic_block_size),
        size(normalize(_basic_block_size, _size)),
        data(std::malloc(_size + sizeof(SegmentHeader))),
        freeList(){
    assert(data != nullptr);
    auto* first = new(data)SegmentHeader{size};
    assert(freeList.Add(first));
}

MyAllocator::~MyAllocator(){
    std::free(data);
}

Addr MyAllocator::Malloc(size_t _length){
    auto real_length = normalize(block_size, _length + sizeof(SegmentHeader));
    auto segment = freeList.FindFirstFreeBiggerThan(real_length);
    if(segment == nullptr){
        return nullptr;
    }
    freeList.Remove(segment);
    if(real_length - segment->getLength() <= sizeof(SegmentHeader)){
        segment->setFree(false);
    }
    else{
        auto old_length = segment->getLength();
        segment = new(segment)SegmentHeader{real_length, false};
        auto next_segment = new(reinterpret_cast<SegmentHeader*>(uintptr_t(segment) + real_length))SegmentHeader{
                old_length - real_length
        };
        freeList.Add(next_segment);
    }
    return reinterpret_cast<Addr>(uintptr_t(segment) + sizeof(SegmentHeader));
}

bool MyAllocator::Free(Addr _a){
    auto real_addr = reinterpret_cast<SegmentHeader*>(uintptr_t(_a) - sizeof(SegmentHeader));
    real_addr->CheckValid();
    real_addr->setFree(true);
    return freeList.Add(real_addr);
}
