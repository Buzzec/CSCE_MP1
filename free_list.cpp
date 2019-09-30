#include <iostream>
#include <cassert>
#include "free_list.hpp"

segment_header::segment_header(size_t size_index, bool parent_is_left, bool inherited_is_left, bool is_free,
                               segment_header* next, segment_header* prev){
    this->size_index = size_index;
    flags = COOKIE_FLAG +
            static_cast<uint8_t>(parent_is_left) * PARENT_LEFT_FLAG +
            static_cast<uint8_t>(inherited_is_left) * INHERITED_LEFT_FLAG +
            static_cast<uint8_t>(is_free) * FREE_FLAG;
    this->next = next;
    this->prev = prev;
#ifndef NDEBUG
    check_valid();
#endif
}

void segment_header::check_valid(){
    assert(COOKIE_FLAG == (COOKIE_FLAG & flags));
}

size_t segment_header::get_size_index(){
    return size_index;
}

bool segment_header::is_parent_left(){
    return flags & PARENT_LEFT_FLAG;
}

bool segment_header::is_inherited_left(){
    return flags & INHERITED_LEFT_FLAG;
}

bool segment_header::is_free(){
    return flags & FREE_FLAG;
}

void segment_header::set_free(bool is_free){
    if(is_free != this->is_free()){
        if(this->is_free()){
            flags -= FREE_FLAG;
        }
        else{
            flags += FREE_FLAG;
        }
    }
}

segment_header::~segment_header() = default;

free_list::free_list(uint8_t size_index) : head(nullptr), size_index(size_index){}

bool free_list::remove(segment_header* segment){
#ifndef NDEBUG
    if(segment == nullptr){
        return false;
    }
    auto part = head;
    while(part != nullptr && part != segment){
        part = part->next;
    }
    if(part != segment){
        return false;
    }
#endif
    if(segment->prev != nullptr){
        segment->prev->next = segment->next;
    }
    else{
        head = segment->next;
    }
    if(segment->next != nullptr){
        segment->next->prev = segment->prev;
    }
    segment->next = nullptr;
    segment->prev = nullptr;
    segment->set_free(false);
    return true;
}

bool free_list::add(void* segment, bool parent_is_left, bool inherited_is_left){
#ifndef NDEBUG
    if(segment == nullptr){
        return false;
    }
#endif
    auto segment_real = new(segment)segment_header{size_index, parent_is_left, inherited_is_left};
    if(head != nullptr){
        head->prev = segment_real;
    }
    segment_real->next = head;
    head = segment_real;
    return true;
}

uint8_t free_list::get_size_index() const{
    return size_index;
}

bool free_list::is_empty() const{
    return head == nullptr;
}

segment_header* free_list::pop_head(){
    auto out = head;
    remove(head);
    return out;
}

free_list::~free_list() = default;
