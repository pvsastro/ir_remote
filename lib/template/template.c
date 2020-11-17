/**
 * @file  template.c
 *
 * @brief Device driver template
 *
 * <Detailed description here>
 */

#include "template.h"
#include "class.h"

/**
 * @brief Template descriptor
 *
 * Internal structure used to manage the device.
 */
struct template_desc {
    const struct class *class;
    /* Add template descriptor data here */
};

static void *template_ctor(void *data);
static void template_dtor(void *self);
static int template_ioctl(void *self, int cmd, void *data);

/**
 */
static void *template_ctor(void *data)
{
    void *ret = NULL;

    /* Create and return the device descriptor here */

    return ret;
}

/**
 */
static void template_dtor(void *self)
{
    struct template_desc *desc= self;
    /* Use the descriptor to perform clean up */
    UNUSED(desc);
}

/**
 */
static int template_ioctl(void *self, int cmd, void *data)
{
    struct template_desc *desc = self;
    int ret = EFAIL;

    UNUSED(desc);

    if (IOC_TYPE(cmd) == IOCTL_TEMPLATE) {
        switch (IOC_NR(cmd)) {
        case IOCTL_TEMPLATE_TEST1: {
            break;
        }
        case IOCTL_TEMPLATE_TEST2:
            break;
        default:
            break;
        }
    }

    return ret;
}

/**
 * @brief Template class
 */
static const struct class _template = {
    template_ctor,
    template_dtor,
    template_ioctl,
};

const void *template = &_template;

