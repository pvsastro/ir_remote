/**
 * @file  class.h
 *
 * @brief Device class specification
 *
 * Operations common to all the devices are collected into this class
 * structure.
 */

#ifndef __CLASS_H__
#define __CLASS_H__

/* Top-level class */
struct class {
    void *(* ctor)(void *data); /* Constructor */
    void (* dtor)(void *self);  /* Destructor */
    int (* ioctl)(void *self, int req, void *data); /* Device i/o control */
};

#endif /* __CLASS_H__ */

