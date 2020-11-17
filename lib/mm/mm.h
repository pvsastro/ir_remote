/**
 * @file  mm.h
 *
 * @brief Memory Manager Subsystem
 */

#ifndef __MM_H__
#define __MM_H__

#include "common.h"

/**
 */
void *mm_alloc(size_t size);

/**
 */
void mm_free(void *ptr);

#endif /* __MM_H__*/

