/**
 * @file  gpio_chip.h
 *
 * @brief GPIO device driver declarations
 *
 * This file declares data types to be used by the GPIO controller drivers.
 */

#ifndef __GPIO_CHIP_H__
#define __GPIO_CHIP_H__

#include "common.h"

#define CHIP_TABLE(_label, _nr, _set_val, _get_val, _set_cfg)   \
    {                                                           \
        .chip_label = _label,                                   \
        .nr_gpios = _nr,                                        \
        .set_value = _set_val,                                  \
        .get_value = _get_val,                                  \
        .set_config = _set_cfg,                                 \
    }

/**
 * @brief GPIO controller descriptor
 */
struct gpio_chip {
    const char *chip_label;     /*!< GPIO controller name */
    void (* set_value)(const struct gpio_chip *chip, uint16_t offset, int value);
    int (* get_value)(const struct gpio_chip *chip, uint16_t offset);
    int (* set_config)(const struct gpio_chip *chip, uint16_t offset, uint16_t config);
    unsigned int nr_gpios;      /*!< Number of GPIOs handled by this controller */
};

/**
 * @brief GPIO controller descriptor array
 */
struct gpio_chip_table {
    struct list_head list;
    unsigned nr_chips;              /*!< Number of GPIO controllers */
    const struct gpio_chip chip[];  /*!< List of GPIO controller descriptors */
};

void gpio_add_chip_table(struct gpio_chip_table *table);

#endif /* __GPIO_CHIP_H__ */

