/***************************************************************************//**

  @file         hashtabletest.c

  @author       Stephen Brennan

  @date         Created Tuesday,  5 December 2013

  @brief        A test of the hash table.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
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

#include <stdio.h>

#include "tests.h"
#include "libstephen/ut.h"
#include "libstephen/ht.h"

#define TEST_PAIRS 5

char *test_keys[] = {
  "first key",
  "second key",
  "third key",
  "fourth key",
  "fifth key"
};

char *test_values[] = {
  "first value",
  "second value",
  "third value",
  "fourth value",
  "fifth value"
};

int ht_test_deletions = 0;

void ht_test_deleter(DATA dValue)
{
  ht_test_deletions++;
}

unsigned int ht_test_constant_hash(DATA dKey)
{
  // return random_die_roll();
  return 4;
}

unsigned int ht_test_linear_hash(DATA dKey)
{
  return (unsigned int) dKey.data_llint;
}

/**
   This doesn't really just test insert.  It also tests create and get.  But I
   can't really isolate *just* insert.
 */
int ht_test_insert()
{
  smb_status status;
  DATA key, value;
  int i;

  smb_ht *table = ht_create(&ht_string_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    TEST_ASLINE(test_values[i] == ht_get(table, key, &status).data_ptr);
    TEST_ASLINE(status == SMB_SUCCESS);
  }

  // ht_print(table, 0);

  ht_delete(table);
  return 0;
}

int ht_test_remove()
{
  smb_status status;
  DATA key, value;
  int i;
  smb_ht *table = ht_create(&ht_string_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  ht_test_deletions = 0;

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
  }

  TEST_ASLINE(table->length == TEST_PAIRS);

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(test_values[i] == value.data_ptr);
    ht_remove_act(table, key, ht_test_deleter, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(table->length == TEST_PAIRS - i - 1);
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASLINE(ht_test_deletions == TEST_PAIRS);
  return 0;
}

/*
  This test expecs a NOT_FOUND_ERROR
 */
int ht_test_remove_invalid()
{
  smb_status status;
  DATA key, value;
  int i;

  smb_ht *table = ht_create(&ht_string_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);

  key.data_ptr = "invalid key";
  value = ht_get(table, key, &status);
  TEST_ASLINE(status == SMB_NOT_FOUND_ERROR);

  ht_delete(table);
  return 0;
}

/**
   This test intentionally uses a very bad hash function that returns a
   constant.  This way, I can test whether the insertion and removal for large
   buckets is working.
 */
int ht_test_buckets()
{
  smb_status status;
  DATA key, value;
  int i;
  smb_ht *table = ht_create(&ht_test_constant_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  ht_test_deletions = 0;

  for (i = 0; i < 20; i++) {
    key.data_llint = i;
    value.data_llint = -i;
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(table->length == i+1);
  }

  //ht_print(table, 0);

  // Remove one from the middle of the bucket list.
  key.data_llint = 10;
  ht_remove_act(table, key, ht_test_deleter, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  TEST_ASLINE(table->length == 19);

  //ht_print(table, 0);

  // Remove one from the beginning of the bucket list.
  key.data_llint = 0;
  ht_remove_act(table, key, ht_test_deleter, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  TEST_ASLINE(table->length == 18);

  //ht_print(table, 0);

  // Remove from the end of the list.
  key.data_llint = 19;
  ht_remove_act(table, key, ht_test_deleter, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  TEST_ASLINE(table->length == 17);

  //ht_print(table, 0);

  // Verify that the other items are still there.
  for (i = 1; i < 10; i++) {
    key.data_llint = i;
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(value.data_llint == -i);
  }

  for (i = 11; i < 19; i++) {
    key.data_llint = i;
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(value.data_llint == -i);
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASLINE(ht_test_deletions == 20);
  return 0;
}

/**
   This test adds to the hash table until it is forced to reallocate.  Then it
   checks that every value is still accessible.
 */
int ht_test_resize()
{
  smb_status status;
  DATA key, value;
  int i;
  // Truncating addition will trim this to the number just before expanding.
  int last_stable = 1 + (int) (HASH_TABLE_INITIAL_SIZE * HASH_TABLE_MAX_LOAD_FACTOR);
  smb_ht *table = ht_create(ht_test_linear_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  ht_test_deletions = 0;

  for (i = 0; i < last_stable; i++) {
    key.data_llint = i;
    value.data_llint = -i;
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(table->allocated == HASH_TABLE_INITIAL_SIZE);
    TEST_ASLINE(table->length == i + 1);
  }

  //ht_print(table, 1);

  key.data_llint = last_stable;
  value.data_llint = -last_stable;
  ht_insert(table, key, value, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  TEST_ASLINE(table->allocated > HASH_TABLE_INITIAL_SIZE);
  TEST_ASLINE(table->length == last_stable + 1);

  //ht_print(table, 1);

  for (i = 0; i <= last_stable; i++) {
    key.data_llint = i;
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    TEST_ASLINE(value.data_llint == -i);
  }

  ht_delete_act(table, ht_test_deleter);
  TEST_ASLINE(ht_test_deletions == last_stable + 1);
  return 0;
}

int ht_test_duplicate()
{
  smb_status status;
  DATA key, value;
  int i;
  char *newKey = "not the first value";
  smb_ht *table = ht_create(ht_string_hash, &data_compare_string, &status);
  TEST_ASLINE(status == SMB_SUCCESS);
  ht_test_deletions = 0;

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value.data_ptr = test_values[i];
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
  }

  //ht_print(table, 0);

  for (i = 0; i < TEST_PAIRS; i += 2) {
    TEST_ASLINE(table->length == TEST_PAIRS);

    key.data_ptr = test_keys[i];
    value.data_ptr = newKey;
    ht_insert(table, key, value, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);

    TEST_ASLINE(value.data_ptr == newKey);
  }

  for (i = 0; i < TEST_PAIRS; i++) {
    key.data_ptr = test_keys[i];
    value = ht_get(table, key, &status);
    TEST_ASLINE(status == SMB_SUCCESS);
    if (i % 2 == 1) {
      TEST_ASLINE(value.data_ptr == test_values[i]);
    } else {
      TEST_ASLINE(value.data_ptr == newKey);
    }
  }

  //ht_print(table, 0);

  ht_delete_act(table, ht_test_deleter);
  TEST_ASLINE(ht_test_deletions == TEST_PAIRS);
  return 0;
}

void hash_table_test()
{
  smb_ut_group *group = su_create_test_group("hash table");

  smb_ut_test *insert = su_create_test("insert", ht_test_insert, 1);
  su_add_test(group, insert);

  smb_ut_test *remove = su_create_test("remove", ht_test_remove, 1);
  su_add_test(group, remove);

  smb_ut_test *remove_invalid = su_create_test("remove_invalid", ht_test_remove_invalid, 1);
  su_add_test(group, remove_invalid);

  smb_ut_test *buckets = su_create_test("buckets", ht_test_buckets, 1);
  su_add_test(group, buckets);

  smb_ut_test *resize = su_create_test("resize", ht_test_resize, 1);
  su_add_test(group, resize);

  smb_ut_test *duplicate = su_create_test("duplicate", ht_test_duplicate, 1);
  su_add_test(group, duplicate);

  su_run_group(group);
  su_delete_group(group);
}
