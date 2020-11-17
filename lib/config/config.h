#ifndef __CONFIG_H__
#define __CONFIG_H__

/**
 * @brief List of IOCTL magic numbers
 *
 * These numbers should be unique for each device driver and forms a
 * part of the IOCTL number (used to denote a device I/O operation).
 */
#define IOCTL_TEMPLATE          0
#define IOCTL_BUFFER            1
#define IOCTL_GPIO              2

/**
 * @brief Data type for the buffer item
 *
 * This is the type for each item of the buffer. App has to define
 * this data type as per it's requirements.
 */
typedef uint8_t buffer_elem_t;

#endif /* __CONFIG_H__ */

