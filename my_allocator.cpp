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

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) : data(std::malloc(_size)), offset(0), size(_size){}

MyAllocator::~MyAllocator(){
    std::free(data);
}

Addr MyAllocator::Malloc(size_t _length){
    if(_length == 0 || offset + _length > size){
        return nullptr;
    }
    else{
        void* out = reinterpret_cast<void*>(uintptr_t(data) + offset);
        offset += _length;
        return out;
    }
}

bool MyAllocator::Free(Addr _a){
    return true;
}

