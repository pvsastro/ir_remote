/**
 * @file  new.c
 *
 * @brief Common object manipulation API
 *
 */

#include "common.h"
#include "class.h"

/**
 */
void *new(const void *class, void *data)
{
    const struct class *c = class;

    /* All the classes should have a constructor */
    return c->ctor(data);
}

/**
 */
void delete(void *self)
{
    struct class **c = self;

    if (self && *c && (*c)->dtor)
        (*c)->dtor(self);
}

/**
 */
int ioctl(void *self, int req, void *data)
{
    struct class **c = self;
    int ret = EFAIL;

    if (self && *c && (*c)->ioctl)
        ret = (*c)->ioctl(self, req, data);

    return ret;
}

