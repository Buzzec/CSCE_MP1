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

/**
 *
 * @param free_lists
 * @param num_blocks
 * @return actual number of blocks
 */
size_t fill_free_lists(std::vector<std::pair<size_t, free_list>>& free_lists, size_t num_blocks){
    free_lists.emplace_back(1, free_list{0});
    free_lists.emplace_back(1, free_list{1});
    uint8_t index = 2;
    while(free_lists.back().first < num_blocks){
        size_t size = free_lists.back().first + free_lists.at(free_lists.size() - 2).first;
        free_lists.emplace_back(size, free_list{index});
        ++index;
    }
    return free_lists.back().first;
}

my_allocator::my_allocator(size_t block_size, size_t num_bytes) :
        free_lists(),
        block_size(0),
        num_blocks(0),
        data(nullptr){
    this->block_size = block_size;
    num_blocks = fill_free_lists(free_lists, normalize(block_size, num_bytes));
    data = std::malloc(this->block_size * this->num_blocks);
    assert(data != nullptr);
    free_lists.back().second.add(data, true, true);
}

my_allocator::~my_allocator(){
    std::free(data);
}

Addr my_allocator::malloc(size_t num_bytes){
    size_t target_num_blocks = normalize(block_size, num_bytes + sizeof(segment_header));
    if(target_num_blocks > free_lists.back().first){
        return nullptr;
    }
    uint8_t target_index = 0;
    for(; free_lists.at(target_index).first < target_num_blocks; target_index++);
    uint8_t first_free_index = target_index;
    for(; first_free_index < free_lists.size() &&
          free_lists.at(first_free_index).second.is_empty(); first_free_index++);
    if(first_free_index == free_lists.size()){
        return nullptr;
    }
    if(first_free_index == target_index || (target_index == 0 && first_free_index == 1)){
        auto out = free_lists.at(first_free_index).second.pop_head();
        return reinterpret_cast<Addr>(uintptr_t(out) + sizeof(segment_header));
    }
    auto splitee = free_lists.at(first_free_index).second.pop_head();
    free_lists.at(first_free_index - 1).second.add(
            reinterpret_cast<void*>(uintptr_t(splitee) + free_lists.at(first_free_index - 2).first * block_size), false,
            splitee->is_inherited_left());
    free_lists.at(first_free_index - 2).second.add(splitee, true, splitee->is_parent_left());
    return malloc(num_bytes);
}

bool my_allocator::free(Addr _a){
    if(_a == nullptr){
        return false;
    }
    auto segment = reinterpret_cast<segment_header*>(uintptr_t(_a) - sizeof(segment_header));
    segment->check_valid();
    free_lists.at(segment->get_size_index()).second.add(segment, segment->is_parent_left(),
                                                        segment->is_inherited_left());
    if(segment->get_size_index() < free_lists.size() - 1){
        segment_header* left_segment;
        segment_header* right_segment;
        if(segment->is_parent_left()){
            left_segment = segment;
            right_segment = reinterpret_cast<segment_header*>(uintptr_t(segment) +
                                                              free_lists.at(segment->get_size_index()).first *
                                                              block_size);
        }
        else{
            left_segment = reinterpret_cast<segment_header*>(uintptr_t(segment) +
                                                             free_lists.at(segment->get_size_index() - 1).first *
                                                             block_size);
            right_segment = segment;
        }
        if(!left_segment->is_free() || !right_segment->is_free() ||
           right_segment->get_size_index() - left_segment->get_size_index() != 1){
            return true;
        }
        assert(free_lists.at(left_segment->get_size_index()).second.remove(left_segment));
        assert(free_lists.at(right_segment->get_size_index()).second.remove(right_segment));
        new(left_segment)segment_header{left_segment->get_size_index() + 2, left_segment->is_inherited_left(),
                                        right_segment->is_inherited_left(), false};
        return free(reinterpret_cast<Addr>(uintptr_t(left_segment) + sizeof(segment_header)));
    }
    return true;
}
