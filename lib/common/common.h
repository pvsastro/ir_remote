/**
 * @file  common.h
 *
 * @brief Common includes
 *
 * Header to include the required common files for all the lib sources.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#include "types.h"
#include "errno.h"
#include "atomic.h"
#include "new.h"
#include "config.h"
#include "ioctl.h"
#include "mm.h"

/*!< Indicate unused function parameter */
#ifndef UNUSED
#define UNUSED(x)   ((void)(x))
#endif

/*!< Convert number into bit map */
#define BIT(nr)     (1UL << (nr))
#define BITS(h, l)  ()

/**
 * @brief Return the offset of the structure member
 */
#define offsetof(type, member)              ((size_t)&((type *)0)->member)

/**
 * @brief Return the address of the containing structure
 */
#define container_of(ptr, type, member)     ((type *)((char *)(ptr) - offsetof(type, member)))

#endif /* __COMMON_H__*/

