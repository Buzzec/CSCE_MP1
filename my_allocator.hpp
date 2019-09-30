/* 
    File: my_allocator.hpp

    Author: Brett Etter
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : 9/5/19

    Modified:

*/

#ifndef _my_allocator_hpp_                   // include file only once
#define _my_allocator_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include <vector>
#include "free_list.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

typedef void* Addr;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   MyAllocator */
/*--------------------------------------------------------------------------*/

class my_allocator{

private:
    void* data;
    std::vector<std::pair<size_t, free_list>> free_lists;
    size_t block_size;
    size_t num_blocks;

public:
    my_allocator(size_t block_size, size_t num_bytes);
    /* This function initializes the memory allocator and makes a portion of
       ’_size’ bytes available. The allocator uses a ’_basic_block_size’ as
       its minimal unit of allocation.
       NOTE: In the old days one used 'unsigned int' for the size. The C
       Standards Committee has added the 'size_t' data type to ensure to
       represent memory sizes. This makes the code more portable. Depending
       on the architecture, 'size_t' may or may not be the same
       as 'unsigned int'.
    */

    ~my_allocator();
    /* This function returns any allocated memory to the operating system.
    */

    Addr malloc(size_t num_bytes);
    /* Allocate _length number of bytes of free memory and returns the
       address of the allocated portion. Returns nullptr when out of memory.
    */

    bool free(Addr _a);
    /* Frees the section of physical memory previously allocated
       using ’Malloc’. Returns true if everything ok. */
};

#endif 
