/**
 * @file  gpio_board.h
 *
 * @brief Configure and associate GPIOs on the board to the controllers
 */

#ifndef __GPIO_BOARD_H__
#define __GPIO_BOARD_H__

#include "common.h"

#define GPIO_LOOKUP(_label, _offset, _id, _flags)         \
    {                                                     \
        .chip_label = _label,                             \
        .offset = _offset,                                \
        .id = _id,                                        \
        .flags = _flags,                                  \
    }

/**
 * @brief Associate GPIO to the controller and the board configuration
 */
struct gpio_lookup {
    const char *chip_label; /*!< GPIO controller name */
    uint16_t offset;        /*!< GPIO number relative to the controller */
    uint16_t id;            /*!< GPIO id as referenced by the app */
    uint16_t flags;         /*!< Flags defining the way GPIOs are connected on the board */
};

/**
 * @brief GPIO board config look up table
 */
struct gpio_lookup_table {
    struct list_head list;
    unsigned nr_items;                /*!< Number of items in the lookup table */
    const struct gpio_lookup table[]; /*!< Table entries */
};

/**
 */
void gpio_add_lookup_table(struct gpio_lookup_table *table);

#endif /* __GPIO_BOARD_H__*/

