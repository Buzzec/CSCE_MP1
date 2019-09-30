#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc30-c"
#pragma ide diagnostic ignored "bugprone-narrowing-conversions"


/*
    File: memtest.c

    Author: R. Bettati
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : 2019/09/02

    Test program for the memory allocator in MP1 of CSCE 313.
    This program reads and processes the command-line arguments and then
    exercises the memory allocator by allocating and de-allocating memory
    as part of the recursive invocations in the function 'ackerman', which 
    is famous for being very highly recursive.
*/
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <cstring>
#include <ctime>
#include <cassert>
#include <sys/time.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)
#include <getopt.h>
#include <climits>

#include "my_allocator.hpp"

/*--------------------------------------------------------------------------*/
/* NAME SPACES */
/*--------------------------------------------------------------------------*/

//using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* GLOBAL DATA */
/*--------------------------------------------------------------------------*/

my_allocator* my_allocator;
/* We are defining our allocator as a global resource. */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void ackerman_main(size_t _block_size, size_t _mem_size);
/* Repeatedly asks user for two parameters, 'a' and 'b', and computes the 
   result of the (highly recursive!) ackerman function. 
   During every recursion step, it allocates and de-allocates a portion 
   of memory with the use of the memory allocator defined in module 
   "my_allocator.H".
*/

int ackerman(int a, int b);
/* This is the implementation of the Ackerman function. The function itself 
   is very simple (just two recursive calls). We use it to exercise the 
   memory allocator (see "MyAllocator::Malloc" and "MyAllocator::Free"). 
   We use two additional calls to "gettimeofday" to measure the elapsed time.
*/

unsigned long num_allocations;
/* Global counter to keep track of the number of allocations exercised
   during one test run.
*/

void print_time_diff(const std::string& _label,
                     const struct timeval& _tp1,
                     const struct timeval& _tp2);
/* Prints to stdout the difference, in seconds and micro seconds, between
   two timevals. 
*/

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char* argv[]){
    int c;
    const char* b_val = "";
    const char* s_val = "";

    while((c = getopt(argc, argv, "b:s:")) != -1){
        switch(c){
            case 'b':
                b_val = optarg;
                break;
            case 's':
                s_val = optarg;
                break;
            case '?':
                if(optopt == 'b' || optopt == 's'){
                    std::cout << "Option -" << optopt << " requires numeric argument" << std::endl;
                }
                else if(isprint(optopt))
                    std::cerr << "Unknown option `-" << optopt << std::endl;
                else
                    std::cerr << "Unknown option character \\x" << optopt << std::endl;
                return 1;
            default:
                std::cerr << "Bad getopt case" << std::endl;
                return 1;
        }
    }
    size_t block_size = 128;
    size_t mem_size = 1ull << 19u; //512k

    if(b_val[0] != '\0'){
        auto val = strtoull(b_val, nullptr, 10);
        if(val == 0ULL){
            std::cerr << "-b must have numeric or non-zero argument" << std::endl;
            return 1;
        }
        if(val == ULLONG_MAX && errno == ERANGE){
            std::cerr << "-b value too large, rounding down to ULLONG_MAX" << std::endl;
        }
        block_size = val;
    }

    if(s_val[0] != '\0'){
        auto val = strtoull(s_val, nullptr, 10);
        if(val == 0ULL){
            std::cerr << "-s must have numeric or non-zero argument" << std::endl;
            return 1;
        }
        if(val == ULLONG_MAX && errno == ERANGE){
            std::cerr << "-s value too large, rounding down to ULLONG_MAX" << std::endl;
        }
        mem_size = val;
    }

    if(block_size > mem_size){
        std::cerr << "block size > mem size, defaulting both to mem size" << std::endl;
        block_size = mem_size;
    }

//    std::cout << "Block Size: " << block_size << std::endl;
//    std::cout << "Mem Size: " << mem_size << std::endl;

    ackerman_main(block_size, mem_size);
}

/*--------------------------------------------------------------------------*/
/* DRIVER FOR THE ACKERMAN FUNCTION */
/*--------------------------------------------------------------------------*/

void ackerman_main(size_t _block_size, size_t _mem_size){
    /* This function first creates an allocator with given block and memory size.
       It then repeatedly first asks the user for the two parameters
       'a' and 'b' to pass to the Ackerman function, and then invokes the
       Ackerman function.
       Before and after the invocation of the ackerman function, the
       value of the wall clock is taken, and the elapsed time for the
       computation of the ackerman function is displayed.
    */

    int a, b; /* Parameters for the invocation of the Ackerman function. */

    struct timeval tp_start{}; /* Used to compute elapsed time. */
    struct timeval tp_end{};

    for(;;){ /* Loop forever, or until we break. */

        num_allocations = 0;

        std::cout << std::endl;
        std::cout << "Please enter parameters 'a' and 'b' to ackerman function." << std::endl;
        std::cout << "Note that this function takes a long time to compute" << std::endl;
        std::cout << "even for small values. Keep 'a' at or below 3, and 'b'" << std::endl;
        std::cout << "at or below 8." << std::endl;
        std::cout << "Otherwise, the function takes seemingly forever." << std::endl;
        std::cout << "Enter 0 for either 'a' or 'b' in order to exit" << std::endl << std::endl;

        std::cout << "  a = ";
        std::cin >> a;
        if(a <= 0) break;
        std::cout << "  b = ";
        std::cin >> b;
        if(b <= 0) break;

        std::cout << "      a = " << a << ", b = " << b << std::endl;

        my_allocator = new class my_allocator(_block_size, _mem_size);

        assert(gettimeofday(&tp_start, nullptr) == 0);
        /* Assert aborts if there is a problem with gettimeofday.
           We rather die of a horrible death rather than returning
           invalid timing information!
        */

        int result = ackerman(a, b);
        /* We really don't care about the result. It's like Zen:
           It's the way there that counts; not the result that we find there.
        */

        assert(gettimeofday(&tp_end, nullptr) == 0);
        /* (see above) */

        delete my_allocator;

        std::cout << "Result of ackerman(" << a << ", " << b << "):" << result << std::endl;

        print_time_diff("Time taken for computation: ", tp_start, tp_end);

        std::cout << "Number of allocate/free cycles: " << num_allocations << std::endl;
        std::cout << std::endl << std::endl;

    }

    std::cout << "Reached end of Ackerman program. Thank you for using it" << std::endl;

}

/*--------------------------------------------------------------------------*/
/* IMPLEMENTATION OF ACKERMAN FUNCTION */
/*--------------------------------------------------------------------------*/

int ackerman(int a, int b){
/* This is the implementation of the Ackerman function. 
   The function itself is very simple (just two recursive calls). 
   We use it to exercise the memory allocator 
   (see "MyAllocator::Malloc" and "MyAllocator::Free"). 
   We use two additional calls to "gettimeofday" to measure the elapsed time.
*/

    /* Choose randomly the size of the memory to allocate: */
    int to_alloc = ((2u << static_cast<unsigned char>(rand() % 19)) * (rand() % 100)) / 100;
    if(to_alloc < 4) to_alloc = 4;

    int result = 0;

    /* Here we allocate the memory using our own allocator. */
    char* mem = (char*) my_allocator->malloc(to_alloc * sizeof(char));

    num_allocations++;

    if(mem == nullptr){

        std::cerr << "ALLOCATOR FAILED: PROBABLY OUT OF MEMORY." << std::endl;

    }
    else{

        /* Fill the allocated memory with a value.
           This same value will be used later for testing.
        */
        char c = rand() % 128;
        memset(mem, c, to_alloc * sizeof(char));

        /* Here we do the Ackerman recursion thing. */
        if(a == 0)
            result = b + 1;
        else if(b == 0)
            result = ackerman(a - 1, 1);
        else
            result = ackerman(a - 1, ackerman(a, b - 1));

        /* We returned from recursion.
           Let's check if the memory allocated above still has the same value.
        */
        for(int i = 0; i < to_alloc; i++){
            if(mem[i] != c){
                /* The value does not match.
                   It must have been overwritten somewhere. This is very bad.
                */
                std::cout << "Memory checking error!" << std::endl;
                assert(false);
            }
        }

        /* Now we free the memory allocated above; we use our own allocator. */
        assert(my_allocator->free(mem)); /* We crash if Free() fails */
    }

    return result;
    /* Generally the Ackerman function returns a value.
       We don't really care for it.
    */
}

/*--------------------------------------------------------------------------*/
/* UTILITY FUNCTIONS */
/*--------------------------------------------------------------------------*/

void print_time_diff(const std::string& _label,
                     const struct timeval& _tp1,
                     const struct timeval& _tp2){
    /* Prints to stdout the difference, in seconds and micro seconds, between
       two timevals.
    */

    std::cout << _label;
    long sec = _tp2.tv_sec - _tp1.tv_sec;
    long musec = _tp2.tv_usec - _tp1.tv_usec;
    if(musec < 0){
        musec += 1000000;
        sec--;
    }
    std::cout << " [sec = " << sec << ", musec = " << musec << "]" << std::endl;

}


#pragma clang diagnostic pop
