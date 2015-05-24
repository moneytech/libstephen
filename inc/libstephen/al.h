/***************************************************************************//**

  @file         al.h

  @author       Stephen Brennan

  @date         Created Sunday,  3 August 2014

  @brief        Libstephen: Array List

  @copyright    Copyright (c) 2013-2015, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_AL_H
#define LIBSTEPHEN_AL_H

#include "base.h"  /* DATA     */
#include "list.h"  /* smb_list */

/**
   @brief The actual array list data type.

   "Bare" functions return a pointer to this structure.  You should not use any
   of the members, as they are implementation specific and subject to change.
 */
typedef struct smb_al
{
  /**
     @brief The area of memory containing the data.
   */
  DATA *data;

  /**
     @brief The number of items in the list.
   */
  int length;

  /**
     @brief The space allocated for the list.
   */
  int allocated;

} smb_al;

void al_init(smb_al *list);
smb_al *al_create();
void al_destroy(smb_al *list);
void al_delete(smb_al *list);

smb_list al_create_list();
smb_list al_cast_to_list(smb_al *list);

void al_append(smb_al *list, DATA newData);
void al_prepend(smb_al *list, DATA newData);
DATA al_get(const smb_al *list, int index, smb_status *status);
void al_remove(smb_al *list, int index, smb_status *status);
void al_insert(smb_al *list, int index, DATA newData);
void al_set(smb_al *list, int index, DATA newData, smb_status *status);
void al_push_back(smb_al *list, DATA newData);
DATA al_pop_back(smb_al *list, smb_status *status);
DATA al_peek_back(smb_al *list, smb_status *status);
void al_push_front(smb_al *list, DATA newData);
DATA al_pop_front(smb_al *list, smb_status *status);
DATA al_peek_front(smb_al *list, smb_status *status);
int al_length(const smb_al *list);
int al_index_of(const smb_al *list, DATA d, DATA_COMPARE comp);

smb_iter al_get_iter(const smb_al *list);

#endif // LIBSTEPHEN_AL_H