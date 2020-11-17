/**
 * @file  gpio.h
 *
 * @brief Device driver header file gpio
 *
 * Application visible header file to access the GPIOs.
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "common.h"

/**
 * @brief Flags to configure the GPIO
 */
#define GPIO_FLAG_DIR       BIT(0)  /*!< 0 - input, 1 - output */
#define GPIO_FLAG_VAL       BIT(1)  /*!< 0 - de-assert, 1 - assert */
#define GPIO_FLAG_PULL      BIT(3)  /*!< 0 - pull-down, 1 - pull-up */

/**
 * @brief GPIO configuration data
 */
struct gpio_config {
    uint16_t flags; /*!< Flags for configuring this GPIO */
    uint16_t mask;  /*!< Mask to select the flags to configure */
};

/**
 * @brief GPIO descriptor array
 */
struct gpio_descs {
    unsigned int nr_descs;  /*!< Number of descriptors */
    void *desc[];           /*!< List of GPIO descriptors */
};

/**
 * @brief GPIO subsystem IOCTLs
 */
/*!< Set GPIO value */
#define IOCTL_GPIO_SET_VAL          0
/*!< Get GPIO value */
#define IOCTL_GPIO_GET_VAL          1
/*!< Set GPIO configuration */
#define IOCTL_GPIO_SET_CFG          2

/*!< GPIO type definition */
extern const void *gpio;

int gpio_get_array(uint16_t *gpio_list, unsigned nr_gpios, struct gpio_descs *descs);
void gpio_put_array(struct gpio_descs *descs);

#endif /* __GPIO_H__ */

