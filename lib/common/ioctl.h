/**
 * @file  ioctl.h
 *
 * @brief IOCTL manipulation
 *
 * Macros to create and manipulate IOCTL numbers.
 */

#ifndef __IOCTL_H__
#define __IOCTL_H__

#define IOCTL_TYPE_BITS         8
#define IOCTL_NR_BITS           8

#define IOCTL_TYPE_MASK         ((1 << IOCTL_TYPE_BITS) - 1)
#define IOCTL_NR_MASK           ((1 << IOCTL_NR_BITS) - 1)

#define IOCTL_TYPE_SHIFT        0
#define IOCTL_NR_SHIFT          (IOCTL_TYPE_SHIFT + IOCTL_TYPE_BITS)

/* Create IOCTL numbers */
#define IOC(type, nr)           (((type) << IOCTL_TYPE_SHIFT) | \
                                 ((nr) << IOCTL_NR_SHIFT))

/* Decode IOCTL numbers */
#define IOC_TYPE(ioc)            (((ioc) >> IOCTL_TYPE_SHIFT) & IOCTL_TYPE_MASK)
#define IOC_NR(ioc)              (((ioc) >> IOCTL_NR_SHIFT) & IOCTL_NR_MASK)

#endif /* __IOCTL_H__ */

