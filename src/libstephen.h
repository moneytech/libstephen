/*******************************************************************************

  File:         libstephen.h
  
  Author:       Stephen Brennan

  Date Created: Friday, 27 September 2013

  Description: The public interface of libstephen.  Libstephen is a C library
  designed to provide basic data structures and operations to supplement the
  standard C library.  It is mainly a hobby programming project, but on
  completion it should be usable for basing future projects on it.

*******************************************************************************/

#ifndef SMB___LIBSTEPHEN_H_
#define SMB___LIBSTEPHEN_H_

// Required for size_t
#include <stdlib.h>
// Required for uint64_t
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// USEFUL MACROS
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
    Configuration Section
    Comment/Uncomment the #defines to enable or disable functionality
*******************************************************************************/

#define SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_ENABLE_DIAGNOSTIC_PRINTING

/*******************************************************************************
    SMB_ENABLE_MEMORY_DIAGNOSTICS
    All this functionality is enabled when SMB_MEMORY_DIAGNOSTICS is defined.
*******************************************************************************/

// These functions should NOT be called on their own.  The macros should be
// used, as they are macro controlled.
void smb___helper_inc_malloc_counter(size_t number_of_mallocs);
void smb___helper_dec_malloc_counter(size_t number_of_frees);
size_t smb___helper_get_malloc_counter();

/**
   Call this function with the number of bytes every time you allocate memory.
   Alternatively, call it with the number of times you use the malloc function.
   Whichever policy you use, stick to it.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_INCREMENT_MALLOC_COUNTER(x) smb___helper_inc_malloc_counter(x)
#else
#define SMB_INCREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function with the number of bytes every time you free memory.
   Alternatively, call it with the number of times you called free.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_DECREMENT_MALLOC_COUNTER(x) smb___helper_dec_malloc_counter(x)
#else
#define SMB_DECREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function to get the number of bytes currently allocated by my code.

   Returns a size_t representing the value of the malloc counter, which could be
   in bytes or mallocs, depending on your convention.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_GET_MALLOC_COUNTER smb___helper_get_malloc_counter()
#else
#define SMB_GET_MALLOC_COUNTER 0
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_CODE
    Allows writing diagnostic code.  Enabled by SMB_DIAGNOSTIC_CODE
*******************************************************************************/

/**
   Run the given statement only in diagnostic code.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_DIAG_ONLY(x) x
#else
#define SMB_DIAG_ONLY(x)
#endif // SMB_ENABLE_DIAGNOSTIC_CODE

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_PRINTING
*******************************************************************************/

/**
   Pass these parameters to printf only in diagnostic code.  Equivalent to
   SMB_DIAG_ONLY(printf(...)), but SMB_DIAG_PRINT(...) is much nicer.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_PRINTING
#define SMB_DIAG_PRINT(x...) printf(x)
#else
#define SMB_DIAG_PRINT(x...)
#endif // SMB_ENABLE_DIAGNOSTIC_PRINTING


////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLING
////////////////////////////////////////////////////////////////////////////////

// Not for external use
#define ERROR_VAR smb___error_var_
extern unsigned int ERROR_VAR;

// Error codes //

/**
   ALLOCATION_ERROR: Reserved for communicating errors with malloc().
 */
#define ALLOCATION_ERROR 0x0001

/**
   INDEX_ERROR: Set when an provided index is out of range.  Includes incidents
   when you try to pop or peek at an empty list.
 */
#define INDEX_ERROR 0x0002

/**
   NOT_FOUND_ERROR: Set when an item is not found, mainly in the hash table.
 */
#define NOT_FOUND_ERROR 0x0004

/**
   Set the flag corresponding to the given error code.
 */
#define RAISE(x) (ERROR_VAR |= x)

/**
   Test the flag corresponding to the given error code.  Returns 1 if flag set,
   0 if flag cleared.
 */
#define CHECK(x) (ERROR_VAR & x)

/**
   Clear the flag corresponding to the given error code.
 */
#define CLEAR(x) (ERROR_VAR &= (~x))

/**
   Clear the flags for all errors.
 */
#define CLEAR_ALL_ERRORS ERROR_VAR = 0

////////////////////////////////////////////////////////////////////////////////
// UNIT TESTING
////////////////////////////////////////////////////////////////////////////////

/**
   The size of a description text field (in characters) for an smbunit test.
 */
#define SMB_UNIT_DESCRIPTION_SIZE 20

/**
   The max number of unit tests in a single test group.
 */
#define SMB_UNIT_TESTS_PER_GROUP 20

/**
   Defines a single unit test.  Members should be modified with care, preferably
   not at all, except by using the smbunit functions.

   # Members #
   
   - char description[20]: a 20 character null-terminated string that identifies
     this particular test.

   - int (*run)(): function pointer to the test to run.  The function should
     return 0 if the test is successful.

   - int expected_errors: contains error flags to CHECK() for after execution of
     the test.  The test will fail if at least one of the errors in the flag are
     not raised.
 */
typedef struct smb_unit_test
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int (*run)();
  int expected_errors;
  int check_mem_leaks;

} TEST;

/**
   A structure holding a group of unit tests that are all related.  Members
   shouldn't me modified by client code.  All should be managed by the functions
   in smbunit.

   # Members #

   - char description[]: a short description (length defined by
     SMB_UNIT_DESCRIPTION_SIZE) for the test.

   - int num_tests: the number of tests in the group.

   - TEST *tests[]: pointers to the tests contained.  Max tests defined by
     SMB_UNIT_TESTS_PER_GROUP.
 */
typedef struct smb_unit_test_group
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int num_tests;
  TEST *tests[SMB_UNIT_TESTS_PER_GROUP];

} TEST_GROUP;

/**
   Create and return a new unit test.

   # Parameters #

   - char *description: a description of the test.

   - int (*run)(): a function pointer to the test function.

   - int expected_errors: the errors you expect from the test function.  0 if
     non.  You can combine more than one error with &.

   - int check_mem_leaks: whether to check if the mallocs before = mallocs
     after.  0 for no, 1 for yes.

   # Returns #

   A pointer to the new test.
 */
TEST *su_create_test(char *description, int (*run)(), int expected_errors, int check_mem_leaks);

/**
   Create and return a new test group.

   # Parameters #

   - char *description: a short description for the group.

   # Returns #

   A pointer to the test group.
 */
TEST_GROUP *su_create_test_group(char *description);

/**
   Add a test to the given test group.  A maximum of SMB_UNIT_TESTS_PER_GROUP
   may be added to the group.  After the limit is reached, this function fails
   *silently*, so as to prevent interference with the actual tests.

   # Parameters #

   - TEST_GROUP *group: a pointer to the group to add the test to.

   - TEST *test: a pointer to the test.
 */
void su_add_test(TEST_GROUP *group, TEST *test);

/**
   Run the given test.  Tracks memory allocations and thrown errors.  In order
   to do this, all errors are cleared before execution of the test.

   # Parameters #
   
   - TEST *test: the test to run

   # Returns # 
   
   Returns a code based on the execution of the test (and whether or not the
   feature is enabled for the test):

   - Code 0: Test passed all its conditions.

   - Code 1: Test returned a non-zero return code.  This will be reported on
     stdout, along with the specific return code.  The reason for a non-zero
     return code is usually a failed assertion, in which case the code
     corresponds to the assertion number.

   - Code 2: Expected errors not encountered.  The test expected at least one
     error, and none of the expected errors were raised by the function.

   - Code 3: Memory was leaked.  The test returned 0 and all expected errors
     were found (or no errors were expected or found), but memory leaked.
 */
int su_run_test(TEST *test);

/**
   Run a group of tests.  The tests are run sequentially (in the order they were
   added to the group).  If a test fails, the remaining tests are not executed.

   # Parameters #

   - TEST_GROUP *group: a pointer to the TEST_GROUP to run.

   # Returns #

   Returns an integer.  Since the tests are run sequentially via the
   su_run_test() function, it returns 0 if all tests succeeded, or else the
   return code of the failed test from su_run_test().
 */
int su_run_group(TEST_GROUP *group);

/**
   Frees the memory associated with the test, and performs cleanup.  

   Note that no actual cleanup is required by the test, so the only benefit to
   using this function is that it is future-safe (updates to smbunit may require
   cleanup to be performed in this function), and that it automatically calls
   SMB_DECREMENT_MALLOC_COUNTER().  Failing to call that when freeing a test
   WILL result in a detected memory leak if you place appropriate code in the
   main method, but WILL NOT result in a detected memory leak in any tests.
   Remember that no actual memory leak would have actually occurred.

   # Parameters #

   - TEST *test: the test to free
 */
void su_delete_test(TEST *test);

/**
   Free the memory associated with the group AND ALL TESTS WITHIN IT.  You MUST
   use this to delete test groups.  

   Note that if a pointer to a TEST within the TEST_GROUP is already invalid
   (freed), then su_delete_group() assumes that it has been freed and moves on.
   So you may include a single test in more than one group and safely delete
   them both (but after deleting the first group, the test will no longer be
   valid and a segmentation fault will occur if you try to run the second
   group).

   # Parameters #

   TEST_GROUP *group: a pointer to the group to free
 */
void su_delete_group(TEST_GROUP *group);

/**
   Asserts that an expression is true.  If false, returns a given value.

   Note that this is a macro, so using some things (++ and -- operators
   especially) can have unintended results.  Any code in expr will be executed
   every time, but code within retval will only be executed when the assertion
   fails.  As a general rule, do not put any complicated code inside the
   TEST_ASSERT() statement at all.

   # PARAMETERS #

   - expr: The expression to check.  If the expression evaluates to true (that
     is, not 0), the assertion passes.  If the expression evaluates to false
     (that is, zero), the assertion fails.

   - retval: An integer return code.  The return code will be given in the test
     output on failure, so follow a convention that makes it easy to identify
     exactly at what point the code fails.
 */
#define TEST_ASSERT(expr, retval) if(!(expr)) return retval

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DATA TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic data type for storage in the data structures.  Each data type takes
   up 8 bytes of memory.
 */
typedef union long_data {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;

} DATA;

////////////////////////////////////////////////////////////////////////////////
// LINKED LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Node structure for linked list.  This must be exposed in order for other data
   types to be public.  This should not be used by users of the library.
 */
typedef struct ll_node 
{
  struct ll_node *prev;
  struct ll_node *next;
  DATA data;

} NODE;

/**
   The actual linked list data type.  "Bare" functions return a pointer to this
   structure.
 */
typedef struct ll_obj
{
  NODE *head;
  NODE *tail;
  int length;

} LINKED_LIST;

/**
   A linked list iterator.  Do not modify the structure yourself.
 */
typedef struct ll_iter
{
  LINKED_LIST *list;
  NODE *current;
  int index;

} LL_ITERATOR;

///////////////////////////////////////////////////////////////////////////////
// ARRAY LIST
///////////////////////////////////////////////////////////////////////////////

/**
   The actual array list data type.  "Bare" functions return a pointer to this
   structure.  You should not use any of the members, as they are implementation
   specific and subject to change.
 */
typedef struct al_obj
{
  DATA *data;
  int length;
  int allocated;

} ARRAY_LIST;

////////////////////////////////////////////////////////////////////////////////
// HASH TABLE
////////////////////////////////////////////////////////////////////////////////

#define HT_BUCKET struct ht_bucket
HT_BUCKET
{
  DATA key;
  DATA value;
  HT_BUCKET *next;
};

#define HASH_TABLE struct hash_table
HASH_TABLE
{
  int length;
  int allocated;
  unsigned int (*hash)(DATA dData);
  HT_BUCKET **table;
};

#define HASH_TABLE_INITIAL_SIZE 257 // prime number close to 256
#define HASH_TABLE_MAX_LOAD_FACTOR 0.7

////////////////////////////////////////////////////////////////////////////////
// ARGUMENT DATA
////////////////////////////////////////////////////////////////////////////////

/**
   Data structure to store information on arguments passed to the program.
 */
#define MAX_FLAGS 52
#define ARG_DATA struct arg_data
ARG_DATA
{
  uint64_t flags; // bit field for all 52 alphabetical characters
  char *flag_strings[MAX_FLAGS];
  LINKED_LIST *long_flags;
  LINKED_LIST *long_flag_strings;
  LINKED_LIST *bare_strings;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// GENERIC TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic list data structure.
 */
typedef struct smb_list 
{
  // Pointer to the actual structure in memory.  Could be any type.
  void *data;

  // Traditional List functions

  /**
     Append the given data to the given list.

     # Parameters #

     - struct smb_list *l: a pointer to the list to append to

     - DATA newData: the data to add to it.

     # Error Handling #

     Like all main library functions, clears all errors upon entering the
     function.  Can raise an ALLOCATION_ERROR.  On an ALLOCATION_ERROR, the list
     remains valid, but the data is not added to the list.
   */
  void (*append)(struct smb_list *l, DATA newData);

  /**
     Prepend the given data to the beginning of the given list.

     # Parameters #

     - struct smb_list *l: a pointer to the list to append to

     - DATA newData: the data to add to the list

     # Error Handling #

     Clears all errors upon entering the function.  Can raise ALLOCATION_ERROR.
     On an ALLOCATION_ERROR, the list should remain valid, but the data is not
     added to the list.
   */
  void (*prepend)(struct smb_list *l, DATA newData);
  

  /**
     Get the data at the specified index.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index to get

     # Return #

     int: the data at the specified index

     # Error Handling #
     
     Clears all errors on function call.  Can raise an index error.
   */
  DATA (*get)(struct smb_list *l, int index);

  /**
     Set the data at the specified index

     # Parameters # 
     
     - struct smb_list *l: a pointer to the list

     - int index: the index to modify

     - DATA newData: the new data to place at that index

     # Error Handling #

     Clears all errors on function call.  Can raise an index error.
   */
  void (*set)(struct smb_list *l, int index, DATA newData);
  
  /**
     Remove the data at the given index, shifting other entries down.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index to delete

     # Error Handling #

     Clears all errors on function call.  Can raise an index error.
   */
  void (*remove)(struct smb_list *l, int index);

  /**
     Insert data at a specifiend index, moving up all the data at and above that
     index.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index at which to insert

     - DATA newData: the data to insert

     # Error Handling #

     If the index is less than zero, the function prepends.  If the index is
     greater than the size, the function appends.  So no index errors are
     raised.  However, an ALLOCATION_ERROR may be raised.  Clears all errors on
     function call.
   */
  void (*insert)(struct smb_list *l, int index, DATA newData);

  /**
     Delete the entire list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Error Handling #

     No effect on any flags.
   */
  void (*delete)(struct smb_list *l);

  /**
     Return the length of the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #
     
     An integer, the number of entries in the list.

     # Error Handling #

     No effect on any flags.
   */
  int (*length)(struct smb_list *l);

  // Stack/Queue/Deque Functions

  /**
     "Push" (as in a stack) to the back of the list.  Same as append.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - DATA newData: data to push

     # Error Handling #

     Clears all errors on a function call. Can raise an allocation error.
   */
  void (*push_back)(struct smb_list *l, DATA newData);

  /**
     "Pop" (as in a stack) from the back of the list.  This returns the value at
     the end of the list and deletes it from the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*pop_back)(struct smb_list *l);

  /**
     "Peek" (as in a stack) from the back of the list.  This returns the value
     at the end of the list without deleting it.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*peek_back)(struct smb_list *l);
  

  /**
     "Push" (as in a stack) to the front of the list.  Same as prepend.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - DATA newData: data to push

     # Error Handling #

     Clears all errors on a function call. Can raise an allocation error.
   */
  void (*push_front)(struct smb_list *l, DATA newData);

  /**
     "Pop" (as in a stack) from the front of the list.  This returns the value
     at the beginning of the list and deletes it from the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*pop_front)(struct smb_list *l);

  /**
     "Peek" (as in a stack) from the front of the list.  This returns the value
     at the end of the list without deleting it.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*peek_front)(struct smb_list *l);

  // Iterator functions may reside here in the near future.

} LIST;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// "BARE" DATA TYPE FUNCTION DECLARATION
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// LINKED LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Creates a new list with the given data.

   # Parameters #

   - DATA newData: the data to store.

   # Returns #

   A pointer to the new list.

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.  In this case, returns NULL,
   and no memory is leaked.
 */
LINKED_LIST *ll_create(DATA newData);

/**
   Creates a new list empty.

   # Returns #

   A pointer to the new list.

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
LINKED_LIST *ll_create_empty();

/**
   Creates a new list with initial data.  Returns an instance of the interface.

   # Parameters #

   - DATA newData: The initial data

   # Returns #

   A generic list interface object.

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
LIST ll_create_list(DATA newData);

/**
   Create a new list empty.  Returns an instance of the list interface.

   # Returns #

   A generic list interface object.

   # Error Handling #

   Clears all errors. Can raise ALLOCATION_ERROR.
 */
LIST ll_create_empty_list();

/**
   Cast a LINKED_LIST pointer to an instance of the list interface.

   # Parameters #

   - LINKED_LIST *list: the linked list to cast to an generic list.

   # Returns #

   A generic list interface object.
   
   # Error Handling #

   No effect on flags.
 */
LIST ll_cast_to_list(LINKED_LIST *list);

/**
   Append the given data to the end of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   - DATA newData: the data to append

   # Error Handling #

   Clears all errors.  Can raise an ALLOCATION_ERROR.
 */
void ll_append(LINKED_LIST *list, DATA newData);

/**
   Prepend the given data to the beginning of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list
   
   - DATA newData: the data to prepend

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
void ll_prepend(LINKED_LIST *list, DATA newData);

/**
   Push the data to the back of the list.  An alias for ll_append.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   - DATA newData: the data to push

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
void ll_push_back(LINKED_LIST *list, DATA newData);

/**
   Pop data from the back of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   # Returns #

   The data from the back of the list.

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
DATA ll_pop_back(LINKED_LIST *list);

/**
   Peek at the back of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   # Returns #

   The data from the back of the list.

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
DATA ll_peek_back(LINKED_LIST *list);

/**
   Push the data to the front of the list.  An alias for ll_prepend.

   # Parameters #
   
   - LINKED_LIST *list: a pointer to the list

   - DATA newData: the data to push

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
void ll_push_front(LINKED_LIST *list, DATA newData);

/**
   Pop the data from the front of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   # Returns #
   
   The data from the front of the list.

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
DATA ll_pop_front(LINKED_LIST *list);

/**
   Peek at the front of the list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list.

   # Returns #

   The data from the front of the list.

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
DATA ll_peek_front(LINKED_LIST *list);

/**
   Gets the data from the given index.  However, there is no guarantee that the
   index was valid.  An empty DATA object is returned in that case, and an
   INDEX_ERROR is raised.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   - int index: the index to get

   # Return #

   The data at the specified index.

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
DATA ll_get(LINKED_LIST *list, int index);

/**
   Removes the node at the given index.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list.

   - int index: the index to remove

   # Error Handling #

   Clears all errors.  Can raise INDEX_ERROR.
 */
void ll_remove(LINKED_LIST *list, int index);

/**
   Inserts the item at the specified location in the list, pushing back
   elements.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   - int index: the index to insert at

   - DATA newData: the data to insert.

   # Error Handling #

   Clears all errors.  Can raise ALLOCATION_ERROR.
 */
void ll_insert(LINKED_LIST *list, int index, DATA newData);

/**
   Removes the linked list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list.

   # Error Handling #

   No effect.
 */
void ll_delete(LINKED_LIST *list);

/**
   Sets an existing element to a new value.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   - int index: the index to set at

   - DATA newData: the data to set to.

   # Error Handling #

   Clears all errors. Can raise INDEX_ERROR.
 */
void ll_set(LINKED_LIST *list, int index, DATA newData);

/**
   Returns the length of the given list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   # Returns #

   The length of the list.

   # Error Handling #

   No effect.
 */
int ll_length(LINKED_LIST *list);

/**
   Get an iterator for the linked list.

   # Parameters #

   - LINKED_LIST *list: a pointer to the list

   # Returns #
   
   An iterator

   # Error Handling #

   No effect (not yet defined)
 */
LL_ITERATOR ll_get_iter(LINKED_LIST *list);

/**
   Advance the iterator and return the data at it.

   # Parameters #

   - LL_ITERATOR *iterator: the iterator

   # Return #

   The data at the new location of the iterator.
 */
DATA ll_iter_next(LL_ITERATOR *iterator);

/**
   Move the iterator back and return the data at it.

   # Parameters #

   - LL_ITERATOR *iterator: the iterator

   # Return #

   The data at the new location of the iterator
 */
DATA ll_iter_prev(LL_ITERATOR *iterator);

/**
   Get the current data.

   # Parameters #

   - LL_ITERATOR *iterator: the iterator

   # Return #

   The data at the current location of the iterator.
 */
DATA ll_iter_curr(LL_ITERATOR *iterator);

/**
   Check if the iterator can be advanced.

   # Parameters #

   - LL_ITERATOR *iterator: the iterator

   # Return #

   Whether the iterator can be advanced.
 */
int ll_iter_has_next(LL_ITERATOR *iterator);

/**
  Check if the iterator can be moved back.

  # Parameters #

  - LL_ITERATOR *iterator: the iterator

  # Return #
  
  Whether the iterator can be moved back
 */
int ll_iter_has_prev(LL_ITERATOR *iterator);

/**
   Check if the iterator is valid.

   # Parameters #

   - LL_ITERATOR *iterator: the iterator

   # Return #

   Whether the iterator is valid.
 */
int ll_iter_valid(LL_ITERATOR *iterator);

////////////////////////////////////////////////////////////////////////////////
// ARRAY LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Create an ARRAY_LIST initialized with the given first element.
 */
ARRAY_LIST *al_create(DATA newData);

/**
   Create an ARRAY_LIST with no data.
 */
ARRAY_LIST *al_create_empty();

void al_append(ARRAY_LIST *list, DATA newData);

void al_prepend(ARRAY_LIST *list, DATA newData);

DATA al_get(ARRAY_LIST *list, int index);

void al_set(ARRAY_LIST *list, int index, DATA newData);

void al_remove(ARRAY_LIST *list, int index);

void al_insert(ARRAY_LIST *list, int index, DATA newData);

void al_delete(ARRAY_LIST *list);

int al_length(ARRAY_LIST *list);

void al_push_back(ARRAY_LIST *list, DATA newData);

DATA al_pop_back(ARRAY_LIST *list);

DATA al_peek_back(ARRAY_LIST *list);

void al_push_front(ARRAY_LIST *list, DATA newData);

DATA al_pop_back(ARRAY_LIST *list);

DATA al_peek_back(ARRAY_LIST *list);

////////////////////////////////////////////////////////////////////////////////
// HASH TABLE
////////////////////////////////////////////////////////////////////////////////

/**
   Create a hash table.

   # Parameters #

   - int (*hash_function*)(DATA dData): A function that takes one DATA and
     returns a hash value generated from it.  It should be a good hash function.

  # Return #

  A pointer to the new hash table.

  # Error Handling #

  Clears all errors on function call.  If malloc fails, then no hash table is
  created, NULL is returned, and the ALLOCATION_ERROR flag is raised.
 */
HASH_TABLE *ht_create(unsigned int (*hash_function)(DATA dData));

/**
   Insert data into the hash table.  Expands the hash table if the load factor
   is below a threshold.  If the key already exists in the table, then the
   function will overwrite it with the new data provided.

   # Parameters #

   - HASH_TABLE *pTable: Pointer to the hash table.

   - DATA dKey: The key to insert.

   - DATA dValue: The value to insert at the key.

   # Error Handling #

   Clears all errors on function call.  Function call fails with
   ALLOCATION_ERROR if resize fails, or if bucket creation fails.
 */
void ht_insert(HASH_TABLE *pTable, DATA dKey, DATA dValue);

/**
   Remove the key, value pair stored in the hash table.

   # Parameters #

   - HASH_TABLE *pTable: Pointer to the hash table.

   - DATA dKey: key to delete.

   # Error Handling #

   Clears all errors on function call.
 */
void ht_remove(HASH_TABLE *pTable, DATA dKey);

/**
   Return the value associated with the key provided.

   # Parameters #

   - HASH_TABLE const *pTable: Pointer to the hash table.

   - DATA dKey: key whose value to retrieve.

   # Return #

   The value associated the key.

   # Error Handling #

   Clears all errors on function call.  If the key is not found in the table,
   then raises NOT_FOUND_ERROR.
 */
DATA ht_get(HASH_TABLE const *pTable, DATA dKey);

/**
   Return the hash of the data, interpreting it as a string.

   # Parameters #

   - DATA data: The string to hash, assuming that the value contained is a
     char*.

   # Return #

   The hash value of the string.

   # Error Handling #

   No effect.
 */
unsigned int ht_string_hash(DATA data);

/**
   Free the hash table.  No pointers contained in the table will be freed.

   # Parameters #

   - HASH_TABLE *pTable: The table to free.

   # Error Handling #

   No effect.
 */
void ht_delete(HASH_TABLE *pTable);

/**
   Print the entire hash table.

   # Parameters #

   - HASH_HABLE *pTable: The table to print.

   - int full_mode: Whether to print every row in the hash table.

   # Error Handling #
   
   No effect.
 */
void ht_print(HASH_TABLE const *pTable, int full_mode);

////////////////////////////////////////////////////////////////////////////////
// ARGUMENT DATA
////////////////////////////////////////////////////////////////////////////////

/**
   Analyze the argument data passed to the program.  Pass in the argc and argv,
   but make sure to decrement and increment each respective variable so they do
   not include the name of the program.

   # Parameters #

   - int argc: The number of arguments (not including program name).

   - char **argv: The arguments themselves (not including program name).

   # Return #

   A pointer to an ARG_DATA object.  Use provided functions to query the object
   about every desired flag.
 */
ARG_DATA *process_args(int argc, char **argv);

/**
   Delete an ARG_DATA object returned by process_args().

   # Parameters #

   - ARG_DATA *data: The object returned by process_args().
 */
void arg_data_delete(ARG_DATA *data);

/**
   Check whether a flag is raised.

   # Parameters #

   - ARG_DATA *data: The ARG_DATA returned by process_args().

   - char flag: The character flag to check.  Alphabetical only.

   # Return #

   An integer, 0 iff the flag was not set.
 */
int check_flag(ARG_DATA *data, char flag);

/**
   Check whether a long flag appeared.  It must occur verbatim.

   # Parameters #

   - ARG_DATA *data: The ARG_DATA returned by process_args().

   - char *flag: The string flag to check for.

   # Return #

   An integer, 0 iff the flag was not set.
 */
int check_long_flag(ARG_DATA *data, char *flag);

/**
   Check whether a bare string appeared.  It must occur verbatim.

   # Parameters #

   - ARG_DATA *data: The ARG_DATA returned by process_args().

   - char *string: The string to search for.
 */
int check_bare_string(ARG_DATA *data, char *string);

/**
   Return the string parameter associated with the flag.

   # Parameters #

   - ARG_DATA *data: The ARG_DATA returned by process_args().

   - char flag: The flag to find parameters of.

   # Return #

   The parameter of the flag.
 */
char *get_flag_parameter(ARG_DATA *data, char flag);

/**
   Return the string parameter associated with the long string.

   # Parameters #

   - ARG_DATA *data: The ARG_DATA returned by process_args().

   - char *string: The long flag to find parameters of.

   # Return #

   The parameter of the long flag.  NULL if no parameter or if flag not found.
 */
char *get_long_flag_parameter(ARG_DATA *data, char *string);

#endif // SMB___LIBSTEPHEN_H_
