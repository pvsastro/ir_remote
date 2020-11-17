/**
 * @file  buffer.c
 *
 * @brief Buffer Subsystem
 *
 * Data is pushed into the buffer by first incrementing the head pointer and
 * data is popped from the buffer by first incrementing the tail pointer. In
 * this implementation, the data is not pushed into the buffer, if the buffer
 * is full. The buffer is full if the next location to be written has the same
 * index as the current read index. The buffer is empty if the write (head)
 * and read (tail) indexes are the same.
 */

#include "buffer.h"
#include "class.h"

/**
 * @brief Buffer descriptor
 *
 * Internal structure used to manage the buffers.
 */
struct buffer_desc {
    const struct class *class;
    struct buffer_init *init;   /*!< Buffer init parameters */
    unsigned head;              /*!< Buffer index for the writer */
    unsigned tail;              /*!< Buffer index for the reader */
};

static struct buffer_desc *buffer_init(struct buffer_init *init);
static void buffer_deinit(struct buffer_desc *desc);
static void buffer_flush(struct buffer_desc *desc);
static bool buffer_is_empty(struct buffer_desc *desc);
static bool buffer_is_full_(struct buffer_desc *desc, unsigned *next);
static bool buffer_is_full(struct buffer_desc *desc);
static int buffer_push(struct buffer_desc *desc, buffer_elem_t *elem);
static int buffer_pop(struct buffer_desc *desc, buffer_elem_t *elem);

static void *buffer_ctor(void *data);
static void buffer_dtor(void *self);
static int buffer_ioctl(void *self, int cmd, void *data);

/**
 * @brief Initialize the buffer
 *
 * This function will attempt to allocate and initialize a buffer descriptor.
 *
 * @param init Buffer init parameters
 *
 * @return Buffer descriptor, if the buffer allocation is successful.
 *         NULL, if the buffer allocation isnot successful.
 */
static struct buffer_desc *buffer_init(struct buffer_init *init)
{
    struct buffer_desc *desc = NULL;

    /* Create and initialize a buffer descriptor */
    desc = mm_alloc(sizeof(struct buffer_desc));
    if (desc) {
        desc->class = buffer;
        desc->init = init;
        desc->head = desc->tail = 0;

        buffer_flush(desc);
    }

    return desc;
}

/**
 * @brief De-initialize the buffer
 *
 * This function mark the descriptor as not in use.
 *
 * @param buf Buffer descriptor
 */
static void buffer_deinit(struct buffer_desc *desc)
{
    mm_free(desc);
}

/**
 * @brief Flush the buffer
 *
 * This function will reset the internal state of the buffer. After this call
 * the buffer will be empty.
 *
 * @param buf Buffer descriptor
 */
static void buffer_flush(struct buffer_desc *desc)
{
    desc->head = desc->tail = 0;
}

/**
 * @brief Tests if the buffer is empty
 *
 * @param buf Buffer descriptor
 *
 * @return 1 if the buffer is empty.
 *         0 if the buffer is not empty.
 */
static bool buffer_is_empty(struct buffer_desc *desc)
{
    return (desc->head == desc->tail);
}

/**
 * @brief Test if the buffer is full
 *
 * @param buf Buffer descriptor
 * @param next The next index in the buffer to be written is returned in this
 *             param. The index is valid only if this function returns 1 (i.e.,
 *             buffer is not full.
 *
 * @return 1 if the buffer is empty.
 *         0 if the buffer is not empty.
 */
static bool buffer_is_full_(struct buffer_desc *desc, unsigned *next)
{
    *next = (desc->head + 1) % desc->init->nelems;
    return (*next == desc->tail);
}

/**
 * @brief Tests if the buffer is full
 *
 * @param buf Buffer descriptor
 *
 * @return 1, if the buffer is full.
 *         0, if the buffer is not full.
 */
static bool buffer_is_full(struct buffer_desc *desc)
{
    unsigned next;

    return buffer_is_full_(desc, &next);
}

/**
 * @brief Push an element into the buffer
 *
 * The element is not pushed into the buffer, if the buffer is already full. An
 * error code is returned instead.
 *
 * @param buf Buffer descriptor
 * @param elem Element to be pushed into the buffer.
 *
 * @return 0, if the element is pushed into the buffer successfully.
 *         EBUFFER_FULL, if the element is not pushed into the buffer.
 */
static int buffer_push(struct buffer_desc *desc, buffer_elem_t *elem)
{
    unsigned next;

    ATOMIC_START();
    if (!buffer_is_full_(desc, &next)) {
        desc->init->elem[next] = *elem;
        desc->head = next;
        ATOMIC_END();

        return ENO_ERROR;
    } else {
        ATOMIC_END();
        return EBUFFER_FULL;
    }
}

/**
 * @brief Pop an element from the buffer
 *
 * @param buf Buffer descriptor
 * @param elem Memory (allocated by the caller) in which the next element from
 *             the buffer is returned.
 *
 * @return 0, if the element is popped from the buffer successfully.
 *         EBUFFER_EMPTY, if the element is not popped from the buffer.
 */
static int buffer_pop(struct buffer_desc *desc, buffer_elem_t *elem)
{
    ATOMIC_START();
    if (!buffer_is_empty(desc)) {
        unsigned next = (desc->tail + 1) % desc->init->nelems;
        *elem = desc->init->elem[next];
        desc->tail = next;
        ATOMIC_END();

        return ENO_ERROR;
    } else {
        ATOMIC_END();
        return EBUFFER_EMPTY;
    }
}

/**
 * @brief Create and return a buffer descriptor
 */
static void *buffer_ctor(void *data)
{
    struct buffer_init *b = data;
    void *ret = NULL;

    if (b)
        ret = buffer_init(b);

    return ret;
}

/**
 * @brief Delete the buffer descriptor
 */
static void buffer_dtor(void *self)
{
    struct buffer_desc *desc = self;
    buffer_deinit(desc);
}

/**
 * @brief Handle buffer operations
 */
static int buffer_ioctl(void *self, int cmd, void *data)
{
    struct buffer_desc *desc = self;
    int ret = EFAIL;

    if (IOC_TYPE(cmd) == IOCTL_BUFFER) {
        switch (IOC_NR(cmd)) {
        case IOCTL_BUF_PUSH:
            ret = buffer_push(desc, (buffer_elem_t *)data);
            break;
        case IOCTL_BUF_POP:
            ret = buffer_pop(desc, (buffer_elem_t *)data);
            break;
        case IOCTL_BUF_FLUSH:
            buffer_flush(desc);
            ret = 0;
            break;
        case IOCTL_BUF_IS_FULL: {
            bool *b = data;
            *b = buffer_is_full(desc);
            ret = 0;
            break;
        }
        case IOCTL_BUF_IS_EMPTY: {
            bool *b = data;
            *b = buffer_is_empty(desc);
            ret = 0;
            break;
        }
        default:
            break;
        }
    }

    return ret;
}

/**
 * @brief Buffer class
 */
static const struct class _buffer = {
    buffer_ctor,
    buffer_dtor,
    buffer_ioctl,
};

const void *buffer = &_buffer;

/**
 * Unit test code
 */

#ifdef UNIT_TEST

#include <stdio.h>

#define BUFFER_NELEMS       4

static struct buffer_desc *test_buf;
static buffer_elem_t test_buf_elem[BUFFER_NELEMS];

static void buffer_dump(const char *test);
static void buffer_ss_test_err(const char *test, int lineno, int errno);
static int buffer_ss_test(void);

/* If cond is false, print diagnostics and abort the test */
#define TEST_AND_EXIT_ON_FAIL(test, cond)                   \
    do {                                                    \
        if (!(cond)) {                                      \
            buffer_ss_test_err(test, __LINE__, err);        \
            return err;                                     \
        } else {                                            \
            buffer_dump(test);                              \
        }                                                   \
    } while (0)

/**
 * @brief Dump the buffer
 *
 * By printing the data stored in the buffer.
 */
static void buffer_dump(const char *test)
{
    int i;

    printf("%-15s: Head: %d, Tail: %d: ", test, test_buf->head, test_buf->tail);
    for (i = 0;i < BUFFER_NELEMS; i++)
        printf("%2d ", test_buf->init->elem[i]);
    printf("\n");
}

/**
 * @brief Print the error on failure and dump the buffer
 */
static void buffer_ss_test_err(const char *test, int lineno, int errno)
{
    printf("%s failed at line %d with error: %d: ", test, lineno, errno);
    buffer_dump(test);
}

/**
 * @brief Top level buffer subsystem test function
 */
static int buffer_ss_test(void)
{
    struct buffer_init bi = { test_buf_elem, BUFFER_NELEMS, 0 };
    buffer_elem_t test_data;
    int err = EFAIL;

    test_buf = new(buffer, &bi);

    /* Test the buffer status on empty buffer */
    err = EFAIL;
    TEST_AND_EXIT_ON_FAIL("buffer_is_empty", buffer_is_empty(test_buf));
    TEST_AND_EXIT_ON_FAIL("buffer_is_full", !buffer_is_full(test_buf));

    /* Fill the buffer and test for overflow */
    test_data = 10;
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == EBUFFER_FULL);

    /* Test the buffer status on full buffer */
    err = EFAIL;
    TEST_AND_EXIT_ON_FAIL("buffer_is_empty", !buffer_is_empty(test_buf));
    TEST_AND_EXIT_ON_FAIL("buffer_is_full", buffer_is_full(test_buf));

    /* Empty the buffer and test for underflow */
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 10);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 10);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 10);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == EBUFFER_EMPTY && test_data == 10);

    /* Push and pop the buffer in lockstep */
    test_data = 11;
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 11);
    test_data = 12;
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 12);
    test_data = 13;
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 13);
    test_data = 14;
    TEST_AND_EXIT_ON_FAIL("buffer_push",
        (err = buffer_push(test_buf, &test_data)) == ENO_ERROR);
    TEST_AND_EXIT_ON_FAIL("buffer_pop",
        (err = buffer_pop(test_buf, &test_data)) == ENO_ERROR && test_data == 14);

    return ENO_ERROR;
}

/**
 */
int main(void)
{
    printf("Testing Buffer SS\n");
    if (buffer_ss_test() != ENO_ERROR)
        printf("Buffer SS test failed\n");
    else
        printf("Buffer SS test passed\n");

    return 0;
}

#endif /* UNIT_TEST */

