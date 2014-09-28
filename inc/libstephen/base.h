/***************************************************************************//**

  @file         base.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Base Declarations.

  @copyright    Copyright (c) 2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef LIBSTEPHEN_BASE_H
#define LIBSTEPHEN_BASE_H

#include <stdlib.h>  /* size_t  */
#include <stdio.h>   /* fprintf */
#include <stdbool.h> /* bool */

/*******************************************************************************

                                  Diagnostics

*******************************************************************************/

/*
  The debug option enables all diagnostics.
 */
#ifdef SMB_DEBUG
#define SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_ENABLE_DIAGNOSTIC_PRINTING
#endif

/**
   @brief Increments the memory allocation counter.  Don't use this.

   This function is used for memory management.  It is enabled and disabled by
   the preprocessor.  So you should stick to the macros for this.

   @see SMB_INCREMENT_MALLOC_COUNTER

   @param number_of_mallocs The number of bytes allocated.
 */
void smb___helper_inc_malloc_counter(size_t number_of_mallocs);

/**
   @brief Decrements memory allocation counter.  Don't use.

   @see smb___helper_inc_malloc_counter
   @see SMB_DECREMENT_MALLOC_COUNTER

   @param The number of bytes freed.
 */
void smb___helper_dec_malloc_counter(size_t number_of_frees);

/**
   @brief Gets the memory allocation counter.  Don't use.

   @see smb___helper_inc_malloc_counter
   @see SMB_GET_MALLOC_COUNTER

   @returns The number of bytes allocated.
 */
size_t smb___helper_get_malloc_counter();

/**
   @brief Increment the memory allocation counter.

   Call this function with the number of bytes every time you allocate memory.
   This will only do anything if SMB_MEMORY_DIAGNOSTICS is defined.

   @param The number of bytes allocated.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_INCREMENT_MALLOC_COUNTER(x) smb___helper_inc_malloc_counter(x)
#else
#define SMB_INCREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   @brief Decrement the memory allocation couter.

   Call this function with the number of bytes every time you free memory.
   This will only do anything if SMB_MEMORY_DIAGNOSTICS is defined.

   @param The number of bytes freed.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_DECREMENT_MALLOC_COUNTER(x) smb___helper_dec_malloc_counter(x)
#else
#define SMB_DECREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   @brief Get the memory allocation counter.

   Call this function to get the number of bytes currently allocated by my code.
   Only does anything if SMB_MEMORY_DIAGNOSTICS is defined.  If it's not
   defined, evaluates to 0.

   @returns The number of bytes allocated right now.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_GET_MALLOC_COUNTER smb___helper_get_malloc_counter()
#else
#define SMB_GET_MALLOC_COUNTER 0
#endif

/**
   @brief Run this code only if SMB_ENABLE_DIAGNOSTIC_CODE is defined.
 */
#ifdef SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_DIAG_ONLY(x) x
#else
#define SMB_DIAG_ONLY(x)
#endif // SMB_ENABLE_DIAGNOSTIC_CODE

/**
   @brief Print this (printf) only if SMB_ENABLE_DIAGNOSTIC_PRINTING is defined.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_PRINTING
#define SMB_DP(x...) printf(x)
#else
#define SMB_DP(x...)
#endif // SMB_ENABLE_DIAGNOSTIC_PRINTING


/*******************************************************************************

                                 Base Data Type

*******************************************************************************/

/**
   @brief Base data type for the data structures.

   Capable of containing long integers, double precision floats, or pointers.
   Takes up 8 bytes.
 */
typedef union DATA {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;

} DATA;

/**
   @brief A function pointer that takes a DATA and performs an action on it

   The function could count it, call free on it, print it, etc.  Useful for
   stuff like deleting data structures full of items (if they're pointers to
   dynamically allocated data, they'll need to be freed), applying an action to
   every item in a list (e.g. printing), and many more applications.
 */
typedef void (*DATA_ACTION)(DATA);

/**
   @brief A function pointer that takes two DATA and compares them.

   This comparator function is used for two purposes: (1) to check for equality,
   and (2) to order data.  If a particular type of data has no ordering, then it
   is sufficient for the purposes of equality testing to return 0 if equal, and
   1 if not equal.  However, this will fail for any function that uses the
   DATA_COMPARE to order DATA.  Therefore, any function that takes a
   DATA_COMPARE should specify in its documentation whether it will use it for
   equality testing, or for ordering.

   The DATA_COMPARE function shall return 0 iff the two DATA are equal.  It
   shall return a value less than 0 iff the first is less than the second.  It
   shall return a value greater than zero iff the first is greater than the
   second.
 */
typedef int (*DATA_COMPARE)(DATA,DATA);

int data_compare_string(DATA d1, DATA d2);
int data_compare_int(DATA d1, DATA d2);
int data_compare_float(DATA d1, DATA d2);
int data_compare_pointer(DATA d1, DATA d2);

/**
   @brief A function pointer that takes a DATA and prints it.

   This function should not print a trailing newline!  There are higher level
   functions that will deal with that.
 */
typedef void (*DATA_PRINTER)(FILE*, DATA);

void data_printer_string(FILE *f, DATA d);
void data_printer_int(FILE *f, DATA d);
void data_printer_float(FILE *f, DATA d);
void data_printer_pointer(FILE *f, DATA d);


/*******************************************************************************

                               Memory Management

*******************************************************************************/

void *smb___new(size_t amt);
void *smb___renew(void *ptr, size_t newsize, size_t oldsize);
void *smb___free(void *ptr, size_t oldsize);

/**
   @brief A nicer allocation function.

   This macro/function wraps malloc().  It's inspired by the g_new function in
   glib.  It allows you to allocate memory and adjust the allocation counter in
   one call.  Instead of specifying the size of the memory, you specify type and
   number of instances.

   @param type The type of the memory to allocate.
   @param n The number of instances to allocate.
   @returns A pointer to the allocated memory, casted to the correct type.
 */
#define smb_new(type, n) ((type*) smb___new((n) * sizeof(type)))

/**
   @brief A nicer reallocation function.

   This macro/function wraps realloc().  It allows you reallocate memory and
   adjust the allocation counter accordingly.  Exits on failure.

   @param type The type of the memory to reallocate.
   @param ptr The memory to reallocate.
   @param newamt The amount of items to allocate.
   @param oldamt The previous amount of items that were allocated.
   @returns A pointer to the reallocated memory.
 */
#define smb_renew(type, ptr, newamt, oldamt) \
  ((type*) smb___renew(ptr, (newamt) * sizeof(type), (oldamt) * sizeof(type)))

/**
   @brief A nicer free function.

   This macro/function wraps free().  It exits on failure and adjusts the malloc
   counter.

   @param type The type of memory you're freeing.
   @param ptr The memory you're freeing.
   @param amt The number of items you're freeing.
 */
#define smb_free(type, ptr, amt) smb___free(ptr, (amt) * sizeof(type))

/*******************************************************************************

                                 Error Handling

*******************************************************************************/

/**
   @brief An enumeration of all possible status values for libstephen functions.
 */
typedef enum {

  SMB_SUCCESS,
  SMB_ALLOCATION_ERROR,
  SMB_INDEX_ERROR,
  SMB_NOT_FOUND_ERROR,
  SMB_STOP_ITERATION

} smb_status;

char *smb_status_string(smb_status status);

#define PRINT_ERROR_LOC fprintf(stderr, "An error occurred at line %d in file "\
                                "%s (function %s).\n", __LINE__, __FILE__,\
                                __func__)

#endif // LIBSTEPHEN_BASE_H
