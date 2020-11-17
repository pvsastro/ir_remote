/**
 * @file  new.h
 *
 * @brief Common device object manipulation routines
 *
 */

#ifndef __NEW_H__
#define __NEW_H__

/**
 * @brief Create new device object
 *
 * Apps call this routine to perform I/O control with the device.
 *
 * @param self Device descriptor obtained from the call to \a new().
 * @param req  Unique code for the specific I/O request.
 * @param data Additional data for the I/O request. This could be
 *             data sent to the driver for an I/O request or data
 *             returned by the driver in response to an I/O request.
 *
 * @return Device descriptor on success
 *         NULL on failure
 */
void *new(const void *class, void *data);

/**
 * @brief Remove an object
 *
 * Frees the resources allocated for the object.
 *
 * @param self Device descriptor obtained from the call to \a new().
 *
 */
void delete(void *self);

/**
 * @brief Device I/O control
 *
 * Apps call this routine to perform I/O control with the device.
 *
 * @param self Device descriptor obtained from the call to \a new().
 * @param cmd  Unique code for the specific I/O request.
 * @param data Additional data for the I/O request. This could be
 *             data sent to the driver for an I/O request or data
 *             returned by the driver in response to an I/O request.
 *
 * @return 0 on success
 *         error code (< 0) on failure
 */
int ioctl(void *self, int cmd, void *data);

#endif /* __NEW_H__ */

