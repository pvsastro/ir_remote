#ifndef __APP_TYPES_H__
#define __APP_TYPES_H__

#include <stdint.h>
#include <string.h>

/*!< Singly linked list head */
struct list_head {
    struct list_head *next;
};

typedef int bool;

#endif /* __APP_TYPES_H__ */

