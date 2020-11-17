/**
 * @file  gpio.c
 *
 * @brief GPIO device driver
 */

#include "gpio.h"
#include "gpio_chip.h"
#include "gpio_board.h"
#include "class.h"
#include "list.h"

/**
 * @brief GPIO descriptor
 *
 * Internal structure used to manage the GPIOs.
 */
struct gpio_desc {
    const struct class *class;      /*!< Base class (has to be first) */
    struct list_head list;
    const struct gpio_chip *chip;   /*!< Specific controller that is handling this gpio */
    const struct gpio_lookup *lookup;
};

/**
 * @brief List of GPIO board descriptors
 */
static LIST_HEAD(gpio_lookup_table_list);

/**
 * @brief List of GPIO controller descriptors
 */
static LIST_HEAD(gpio_chip_table_list);

/**
 * @brief List of GPIO class descriptors
 */
static LIST_HEAD(gpio_desc_list);

static const struct gpio_lookup *gpio_lookup(uint16_t id);
static const struct gpio_chip *gpio_find_chip(const char *chip_label);
static void gpio_set_value(struct gpio_desc *desc, int value);
static int gpio_get_value(struct gpio_desc *desc);

static void *gpio_ctor(void *data);
static void gpio_dtor(void *self);
static int gpio_ioctl(void *self, int cmd, void *data);

/**
 */
static const struct gpio_lookup *gpio_lookup(uint16_t id)
{
    struct gpio_lookup_table *lut;
    const struct gpio_lookup *lookup = NULL;
    unsigned i;

    /* Obtain the chip label for this GPIO */
    list_for_each_entry(lut, struct gpio_lookup_table, &gpio_lookup_table_list, list) {
        for (i = 0; !lookup && i < lut->nr_items; i++) {
            if (lut->table[i].id == id) {
                lookup = &lut->table[i];
                break;
            }
        }
    }

    return lookup;
}

/**
 */
static const struct gpio_chip *gpio_find_chip(const char *chip_label)
{
    struct gpio_chip_table *lut;
    const struct gpio_chip *chip = NULL;
    unsigned i;

    /* Find the GPIO controller */
    list_for_each_entry(lut, struct gpio_chip_table, &gpio_chip_table_list, list) {
        for (i = 0; !chip && i < lut->nr_chips; i++) {
            if (!strcmp(chip_label, lut->chip[i].chip_label)) {
                chip = &lut->chip[i];
                break;
            }
        }
    }

    return chip;
}

/**
 * TODO: Handle the GPIOs based on flags (open drain/active low/active high cases
 * needs to be handled separately).
 */
static void gpio_set_value(struct gpio_desc *desc, int value)
{
    desc->chip->set_value(desc->chip, desc->lookup->offset, value);
}

/**
 * TODO: Handle the GPIOs based on flags (open drain/active low/active high cases
 * needs to be handled separately).
 */
static int gpio_get_value(struct gpio_desc *desc)
{
    return desc->chip->get_value(desc->chip, desc->lookup->offset);
}

/**
 */
static void *gpio_ctor(void *data)
{
    const struct gpio_lookup *lookup;
    const struct gpio_chip *chip;
    struct gpio_desc *desc = NULL;

    /* Is this GPIO hooked up in the board file */
    lookup = gpio_lookup(*(uint16_t *)data);
    if (!lookup)
        goto gpio_ctor_done;

    /* Find the controller for this GPIO */
    chip = gpio_find_chip(lookup->chip_label);
    if (!chip)
        goto gpio_ctor_done;

    /* Create and initialize a GPIO descriptor */
    desc = mm_alloc(sizeof(struct gpio_desc));
    if (desc) {
        desc->class = gpio;
        desc->chip = chip;
        desc->lookup = lookup;
        list_add(&desc->list, &gpio_desc_list);
    }

gpio_ctor_done:
    return desc;
}

/**
 */
static void gpio_dtor(void *self)
{
    struct gpio_desc *desc= self;

    list_del(&desc->list, &gpio_desc_list);
    mm_free(desc);
}

/**
 */
static int gpio_ioctl(void *self, int cmd, void *data)
{
    struct gpio_desc *desc = self;
    int ret = EFAIL;

    if (IOC_TYPE(cmd) == IOCTL_GPIO) {
        switch (IOC_NR(cmd)) {
        case IOCTL_GPIO_SET_VAL: {
            int *value = (int *)data;
            (void) gpio_set_value(desc, *value);
            ret = ENO_ERROR;
            break;
        }
        case IOCTL_GPIO_GET_VAL: {
            int *value = (int *)data;
            *value = gpio_get_value(desc);
            ret = ENO_ERROR;
            break;
        }
        case IOCTL_GPIO_SET_CFG: {
            break;
        }
        default:
            break;
        }
    }

    return ret;
}

/**
 * @brief GPIO class
 */
static const struct class _gpio = {
    gpio_ctor,
    gpio_dtor,
    gpio_ioctl,
};

const void *gpio = &_gpio;

/**
 */
void gpio_add_lookup_table(struct gpio_lookup_table *table)
{
    list_add(&table->list, &gpio_lookup_table_list);
}

/**
 */
void gpio_add_chip_table(struct gpio_chip_table *table)
{
    list_add(&table->list, &gpio_chip_table_list);
}

/**
 */
int gpio_get_array(uint16_t *gpio_list, unsigned nr_gpios, struct gpio_descs *descs)
{
    int ret = ENO_ERROR;
    unsigned int i;

    for (i = 0; i < nr_gpios; i++) {
        descs->desc[i] = new(gpio, &gpio_list[descs->nr_descs]);

        if (!descs->desc[i]) {
            ret = EFAIL;
            break;
        }

        descs->nr_descs++;
    }

    return ret;
}

/**
 */
void gpio_put_array(struct gpio_descs *descs)
{
    unsigned int i;

    for (i = 0; i < descs->nr_descs; i++)
        delete(descs->desc[i]);
}

#ifdef UNIT_TEST

#include <stdio.h>

/**
 * Board GPIO definitions
 */

/* Number of GPIOs in ports of STM32F4xx */
#define GPIOA_NR_GPIOS      16
#define GPIOB_NR_GPIOS      16
#define GPIOC_NR_GPIOS      16
#define GPIOD_NR_GPIOS      16

/* Base GPIO number for the GPIO ports of STM32F4xx */
#define GPIOA_ID_BASE       0
#define GPIOB_ID_BASE       (GPIOA_ID_BASE + GPIOA_NR_GPIOS)
#define GPIOC_ID_BASE       (GPIOB_ID_BASE + GPIOB_NR_GPIOS)
#define GPIOD_ID_BASE       (GPIOC_ID_BASE + GPIOC_NR_GPIOS)

#define STM32F4XX_NR_GPIOS  (GPIOD_ID_BASE + GPIOD_NR_GPIOS)

/* Helper macro to define the GPIO number */
#define GPIOA_PIN(x)        (GPIOA_ID_BASE + (x))
#define GPIOB_PIN(x)        (GPIOB_ID_BASE + (x))
#define GPIOC_PIN(x)        (GPIOC_ID_BASE + (x))
#define GPIOD_PIN(x)        (GPIOD_ID_BASE + (x))

#define GPIO_AF2_TIM5       (2)

/* GPIO pin id as used by the app */
enum app_gpio_id {
    gpio_ir_rx = 0,
    gpio_ir_tx = 1,
};

static void stm32f4xx_gpio_set_value(const struct gpio_chip *chip, uint16_t offset, int value);
static int stm32f4xx_gpio_get_value(const struct gpio_chip *chip, uint16_t offset);
static int stm32f4xx_gpio_set_config(const struct gpio_chip *chip, uint16_t offset, uint16_t config);

/* GPIO LUT */
static struct gpio_lookup_table gpio_lut = {
    .nr_items = 2,
    .table = {
        GPIO_LOOKUP("stm32gpio", GPIOA_PIN(1 ), gpio_ir_rx, 0),
        GPIO_LOOKUP("stm32gpio", GPIOD_PIN(12), gpio_ir_tx, 0),
    },
};

/**
 * GPIO chip definitions
 */

/* CHIP table */
static struct gpio_chip_table stm32f4xx_gpio_chips = {
    .nr_chips = 1,
    .chip = {
        CHIP_TABLE("stm32gpio", STM32F4XX_NR_GPIOS, stm32f4xx_gpio_set_value, stm32f4xx_gpio_get_value, stm32f4xx_gpio_set_config),
    },
};

/* Emulate GPIOs */
static int gpio_val[STM32F4XX_NR_GPIOS];
/* Emulate GPIO config */
static int gpio_cfg[STM32F4XX_NR_GPIOS];

/**
 */
static void stm32f4xx_gpio_set_value(const struct gpio_chip *chip, uint16_t offset, int value)
{
    if (offset < chip->nr_gpios)
        gpio_val[offset] = value;
}

/**
 */
int stm32f4xx_gpio_get_value(const struct gpio_chip *chip, uint16_t offset)
{
    if (offset < chip->nr_gpios)
        return gpio_val[offset];
    return EFAIL;
}

/**
 */
int stm32f4xx_gpio_set_config(const struct gpio_chip *chip, uint16_t offset, uint16_t config)
{
    if (offset < chip->nr_gpios) {
        gpio_cfg[offset] = config;
        return 0;
    }
    return EFAIL;
}

static int gpio_get_desc_test(void);
static int gpio_ss_test(void);

/* If cond is false, print diagnostics and abort the test */
#define TEST_AND_EXIT_ON_FAIL(test, cond)                   \
    do {                                                    \
        if (!(cond)) {                                      \
            printf("%s failed at line %d with error %d\n",  \
                    test, __LINE__, err);                   \
            return err;                                     \
        }                                                   \
    } while (0)

/**
 * @brief Top level GPIO subsystem test function
 */
static int gpio_get_desc_test(void)
{
    unsigned ir_rx = gpio_ir_rx;
    unsigned ir_tx = gpio_ir_tx;
    unsigned invalid_gpio = 100;
    void *gpio_ir_rx_desc;
    void *gpio_ir_tx_desc;
    void *invalid_desc;
    int err = EFAIL;

    /* Request for GPIO with no board and chip definitions */
    gpio_ir_rx_desc = new(gpio, &ir_rx);

    /* Test NULL is returned */
    TEST_AND_EXIT_ON_FAIL("no_board_and_chip_defs_test", gpio_ir_rx_desc == NULL);

    /* Register the GPIO LUT with the library */
    gpio_add_lookup_table(&gpio_lut);

    /* Request for GPIO with board definition but no chip definition */
    gpio_ir_rx_desc = new(gpio, &ir_rx);

    /* Test valid descriptors are obtained for GPIOs defined in the board file */
    TEST_AND_EXIT_ON_FAIL("no_chip_defs_test", gpio_ir_rx_desc == NULL);

    /* Register the GPIO chip definitions with the library */
    gpio_add_chip_table(&stm32f4xx_gpio_chips);

    /* Request for GPIOs after board and chip definitions are registered */
    gpio_ir_rx_desc = new(gpio, &ir_rx);
    gpio_ir_tx_desc = new(gpio, &ir_tx);

    /* Test valid descriptors are returned */
    TEST_AND_EXIT_ON_FAIL("rx_desc_test", gpio_ir_rx_desc != NULL);
    TEST_AND_EXIT_ON_FAIL("tx_desc_test", gpio_ir_tx_desc != NULL);

    /* Request for invalid GPIO and verify that NULL is returned */
    invalid_desc = new(gpio, &invalid_gpio);
    TEST_AND_EXIT_ON_FAIL("invalid_gpio", invalid_desc == NULL);

    return ENO_ERROR;
}

/**
 * @brief Top level GPIO library test function
 */
static int gpio_ss_test(void)
{
    int err;
    TEST_AND_EXIT_ON_FAIL("gpio_get_desc_test", gpio_get_desc_test() == ENO_ERROR);

    return ENO_ERROR;
}

/**
 */
int main(void)
{
    printf("Testing GPIO SS\n");
    if (gpio_ss_test() != ENO_ERROR)
        printf("GPIO SS test failed\n");
    else
        printf("GPIO SS test passed\n");

    return 0;
}

#endif /* UNIT_TEST */

