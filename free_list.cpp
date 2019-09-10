/* 
    File: free_list.cpp

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the class FreeList.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <cassert>
#include "free_list.hpp"

/*--------------------------------------------------------------------------*/
/* NAME SPACES */
/*--------------------------------------------------------------------------*/

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
/* FUNCTIONS FOR CLASS SegmentHeader */
/*--------------------------------------------------------------------------*/

SegmentHeader::SegmentHeader(size_t _length, bool _is_free, SegmentHeader* _next, SegmentHeader* _prev)
        : length(_length), is_free(_is_free), cookie(COOKIE_VALUE), next(_next), prev(_prev){}

SegmentHeader::~SegmentHeader() = default;


void SegmentHeader::CheckValid(){
    if(cookie != COOKIE_VALUE){
        std::cout << "INVALID SEGMENT HEADER!!" << std::endl;
        assert(false);
        // You will need to check with the debugger to see how we got into this
        // predicament.
    }
}

size_t SegmentHeader::getLength(){
    return length;
}

void SegmentHeader::setFree(bool _is_free){
    is_free = _is_free;
}

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() : head(nullptr){}

FreeList::~FreeList() = default;

bool FreeList::Add(SegmentHeader* _segment){
    if(_segment == nullptr){
        return false;
    }

    SegmentHeader* next = head;
    SegmentHeader* prev = nullptr;
    while(_segment > next && next != nullptr){
        prev = next;
        next = next->next;
    }
    _segment->next = next;
    _segment->prev = prev;
    if(next != nullptr){
        next->prev = _segment;
    }
    if(prev != nullptr){
        prev->next = _segment;
    }
    else{
        head = _segment;
    }
    return collapse(_segment);
}

bool FreeList::Remove(SegmentHeader* _segment){
    if(_segment == nullptr){
        return false;
    }

    auto item = head;
    while(item != _segment && item != nullptr){
        item = item->next;
    }
    if(item == _segment){
        if(_segment->prev != nullptr){
            _segment->prev->next = _segment->next;
        }
        else{
            head = _segment->next;
        }
        if(_segment->next != nullptr){
            _segment->next->prev = _segment->prev;
        }
        _segment->next = nullptr;
        _segment->prev = nullptr;
        return true;
    }
    else{
        return false;
    }
}

SegmentHeader* FreeList::FindFirstFreeBiggerThan(size_t length){
    SegmentHeader* item = head;
    while(item != nullptr && item->length < length){
        item = item->next;
    }
    return item;
}

bool FreeList::collapse(SegmentHeader* _segment){
    if(_segment == nullptr){
        return false;
    }
    if(_segment->is_free){
        auto prev = _segment->prev;
        auto next = _segment->next;
        if(prev != nullptr && prev->is_free && uintptr_t(prev) + prev->length == uintptr_t(_segment)){
            if(!Remove(prev) || !Remove(_segment)){
                return false;
            }
            return Add(new(prev)SegmentHeader{_segment->length + prev->length});
        }
        if(next != nullptr && next->is_free && uintptr_t(_segment) + _segment->length == uintptr_t(next)){
            if(!Remove(_segment->next) || !Remove(_segment)){
                return false;
            }
            return Add(new(_segment)SegmentHeader{_segment->length + next->length});
        }
    }
    return true;
}
