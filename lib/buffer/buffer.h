/**
 * @file  buffer.h
 *
 * @brief Buffer Subsystem
 *
 * Buffer subsystem provides API to manage circular buffers. The
 * memory for the buffer must be provided by the callers of these
 * API. The provided memory must be one contiguous chunk and is
 * treated as an array of elements of type \a buffer_elem_t by the
 * buffer subsystem. The buffer_elem_t type should be defined by the
 * application.
 *
 * The buffer subsystem can be configured using the flags parameter
 * when creating a new buffer.
 *
 * Constraints:
 * 1. The buffer size is currently constrained to be a power of 2
 *    (for performance reasons).
 * 2. Data being pushed into a full buffer will be ignored.
 * 3. Application can have only one global type for the buffer items.
 * 4. This implementation caters to only a single producer and single
 *    consumer.
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "common.h"

/**
 * @brief Buffer subsystem IOCTLs
 */
/*!< Write an element into the buffer */
#define IOCTL_BUF_PUSH          0
/*!< Read an element from the buffer */
#define IOCTL_BUF_POP           1
/*!< Flush the buffer */
#define IOCTL_BUF_FLUSH         2
/*!< Is the buffer full */
#define IOCTL_BUF_IS_FULL       3
/*!< Is the buffer empty */
#define IOCTL_BUF_IS_EMPTY      4

/**
 * @brief Buffer initializer
 */
struct buffer_init {
    buffer_elem_t *elem; /*!< Pointer to array of buffer elements allocated by the caller */
    unsigned nelems;     /*!< Number of elements in the \a elem array */
    /*!< This field is unused for now */
    uint16_t flags;      /*!< Flags used to configure the buffer */
};

/* Buffer type definition */
extern const void *buffer;

#endif /* __BUFFER_H__*/

