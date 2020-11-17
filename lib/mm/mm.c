/**
 * @file  mm.c
 *
 * @brief Memory Manager Subsystem
 */

#include "mm.h"

#define CONFIG_MM_MEMORY_SIZE       1024

static uint8_t mm[CONFIG_MM_MEMORY_SIZE];
static uint8_t *mm_ptr = mm;

/**
 */
void *mm_alloc(size_t size)
{
    void *new = NULL;

    if ((mm_ptr + size) < &mm[CONFIG_MM_MEMORY_SIZE]) {
        new = mm_ptr;
        mm_ptr += size;
    }

    return new;
}

/**
 */
void mm_free(void *ptr)
{
    UNUSED(ptr);
}

