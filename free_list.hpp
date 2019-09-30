#ifndef _free_list_hpp_
#define _free_list_hpp_

#include <cstdlib>
#include <cstdint>

class free_list;

class segment_header{
private:
    static const uint8_t COOKIE_FLAG = 0b11111000;
    static const uint8_t PARENT_LEFT_FLAG = 0b00000001;
    static const uint8_t INHERITED_LEFT_FLAG = 0b00000010;
    static const uint8_t FREE_FLAG = 0b00000100;
    uint8_t size_index;
    uint8_t flags;
    segment_header* next;
    segment_header* prev;

    friend class free_list;

public:
    segment_header(size_t size_index, bool parent_is_left, bool inherited_is_left, bool _is_free = true,
                   segment_header* _next = nullptr, segment_header* _prev = nullptr);
    ~segment_header();

    void check_valid();

    size_t get_size_index();
    bool is_parent_left();
    bool is_inherited_left();
    bool is_free();

    void set_free(bool is_free);
};

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS  free_list */
/*--------------------------------------------------------------------------*/

class free_list{
private:
    segment_header* head;
    uint8_t size_index;

public:
    explicit free_list(uint8_t size_index);

    ~free_list();

    bool remove(segment_header* segment);
    segment_header* pop_head();
    bool add(void* segment, bool parent_is_left, bool inherited_is_left);

    uint8_t get_size_index() const;
    bool is_empty() const;
};

#endif 
