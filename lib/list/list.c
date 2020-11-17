/**
 * @file  list.c
 *
 * @brief Linked List Library
 */

#include "list.h"

/**
 */
void list_add_tail(struct list_head *new, struct list_head *head)
{
    struct list_head *pos;

    for (pos = head; pos->next != head; pos = pos->next) {
        ;
    }

    pos->next = new;
    new->next = head;
}

/**
 */
void list_add(struct list_head *new, struct list_head *head)
{
    new->next = head->next;
    head->next = new;
}

/**
 */
void list_del(struct list_head *entry, struct list_head *head)
{
    struct list_head *pos;

    for (pos = head; pos->next != head; pos = pos->next) {
        if (pos->next == entry) {
            pos->next = entry->next;
            break;
        }
    }
}

#ifdef UNIT_TEST

#include <stdio.h>

/* Test container data structure */
struct test_data {
    struct list_head list;
    int num;
};

static void list_dump(struct list_head *head);
static int ll_test(void);
static int ll_test_add_del_entry(struct list_head *head);
static int ll_test_add_del_entries(struct list_head *head);
static int ll_test_add_tail(struct list_head *head);
static int ll_test_list_for_each_entry(struct list_head *head);
static int ll_test_list_for_each(struct list_head *head);

/* If cond is false, print diagnostics and abort the test */
#define TEST_AND_EXIT_ON_FAIL(test, cond)                   \
    do {                                                    \
        if (!(cond)) {                                      \
            printf("%s failed at line %d with error: %d\n", \
                   test, __LINE__, err);                    \
            return err;                                     \
        }                                                   \
    } while (0)

/**
 * @brief Dump the list
 *
 * By printing the number stored in the container structure.
 */
static void list_dump(struct list_head *head)
{
    if (!list_empty(head)) {
        struct list_head *pos = head->next;

        while (pos != head) {
            struct test_data *p = container_of(pos, struct test_data, list);

            printf("%d->", p->num);
            pos = pos->next;
        }
        printf("\n");
    } else {
        printf("Empty list\n");
    }
}

/**
 * @brief Test list_add() and list_del() functions
 *
 * 1. Add an entry to the list, verify the head points to the entry
 *    and the entry points to the head.
 * 2. Delete the entry from the list, verify the head points to
 *    itself.
 */
static int ll_test_add_del_entry(struct list_head *head)
{
    struct test_data test;

    printf("%s test\n", __FUNCTION__);

    list_add(&test.list, head);

    /* Test if the head points to the new entry */
    if (head->next != &test.list) {
        printf("List head is not attached to the new entry\n");
        printf("head: %p, head->next: %p, &test.list: %p (line %d)\n",
                head, head->next, &test.list, __LINE__);
        return EFAIL;
    }

    /* Test if the new entry points to the head */
    if (test.list.next != head) {
        printf("New entry is not attached to the list head\n");
        printf("&test.list: %p, test.list.next: %p, head: %p (line %d)\n",
                &test.list, test.list.next, head, __LINE__);
        return EFAIL;
    }

    list_del(&test.list, head);

    /* Test if the list is empty */
    if (!list_empty(head)) {
        printf("Head is not pointing to itself\n");
        printf("head: %p, head->next: %p (line %d)\n", head, head->next,
                __LINE__);
        return EFAIL;
    }

    return ENO_ERROR;
}

/**
 * @brief Test add and delete of multiple entries
 *
 * 1. Add three entries to the list, verify the list structure
 *    by testing all the pointers.
 * 2. Delete the entry from the list, verify the head points to
 *    itself.
 */
static int ll_test_add_del_entries(struct list_head *head)
{
    struct test_data test1 = { .num = 1 };
    struct test_data test2 = { .num = 2 };
    struct test_data test3 = { .num = 3 };

    printf("%s test\n", __FUNCTION__);

    /* Add three entries to the list */
    list_dump(head);
    list_add(&test1.list, head);
    list_dump(head);
    list_add(&test2.list, head);
    list_dump(head);
    list_add(&test3.list, head);
    list_dump(head);

    /* Test if the three entries are added correctly */
    if (head->next != &test3.list &&
        test3.list.next != &test2.list &&
        test2.list.next != &test1.list &&
        test1.list.next != head) {
        printf("List add failed\n");
        printf("head: %p, head->next: %p, &test3.list: %p (line %d)\n",
                head, head->next, &test3.list, __LINE__);
        printf("test3.list.next: %p, &test2.list: %p\n",
                test3.list.next, &test2.list);
        printf("test2.list.next: %p, &test1.list: %p\n",
                test2.list.next, &test1.list);
        printf("test1.list.next: %p\n", test1.list.next);
        return EFAIL;
    }

    /* Delete the three entries */
    list_del(&test1.list, head);
    list_dump(head);
    list_del(&test2.list, head);
    list_dump(head);
    list_del(&test3.list, head);
    list_dump(head);

    /* Test if the list is empty */
    if (!list_empty(head)) {
        printf("Head is not pointing to itself\n");
        printf("head: %p, head->next: %p (line %d)\n", head, head->next,
                __LINE__);
        return EFAIL;
    }

    return ENO_ERROR;
}

/**
 * @brief Test add and delete of multiple entries using list_add_tail
 *
 * 1. Add three entries to the list, verify the list structure
 *    by testing all the pointers.
 * 2. Delete the entry from the list, verify the head points to
 *    itself.
 */
static int ll_test_add_tail(struct list_head *head)
{
    struct test_data test1 = { .num = 1 };
    struct test_data test2 = { .num = 2 };
    struct test_data test3 = { .num = 3 };

    printf("%s test\n", __FUNCTION__);

    /* Add three entries to the list */
    list_dump(head);
    list_add_tail(&test1.list, head);
    list_dump(head);
    list_add_tail(&test2.list, head);
    list_dump(head);
    list_add_tail(&test3.list, head);
    list_dump(head);

    /* Test if the three entries are added correctly */
    if (head->next != &test1.list &&
        test1.list.next != &test2.list &&
        test2.list.next != &test3.list &&
        test3.list.next != head) {
        printf("List add failed\n");
        printf("head: %p, head->next: %p, &test1.list: %p (line %d)\n",
                head, head->next, &test1.list, __LINE__);
        printf("test1.list.next: %p, &test2.list: %p\n",
                test1.list.next, &test2.list);
        printf("test2.list.next: %p, &test3.list: %p\n",
                test2.list.next, &test3.list);
        printf("test3.list.next: %p\n", test3.list.next);
        return EFAIL;
    }

    /* Delete the three entries */
    list_del(&test1.list, head);
    list_dump(head);
    list_del(&test2.list, head);
    list_dump(head);
    list_del(&test3.list, head);
    list_dump(head);

    /* Test if the list is empty */
    if (!list_empty(head)) {
        printf("Head is not pointing to itself\n");
        printf("head: %p, head->next: %p (line %d)\n", head, head->next,
                __LINE__);
        return EFAIL;
    }

    return ENO_ERROR;
}

/**
 * @brief Test list_for_each_entry()
 *
 * 1. Add three entries to the list.
 * 2. Fetch each container structure using list_for_each_entry().
 * 3. Verify the data stored in the container structure (thereby
 *    verifying that the container structure are correctly
 *    returned).
 */
static int ll_test_list_for_each_entry(struct list_head *head)
{
    struct test_data test1 = { .num = 1 };
    struct test_data test2 = { .num = 2 };
    struct test_data test3 = { .num = 3 };
    struct test_data *entry;
    /* Expected data in the container structures */
    int expected[] = { 3, 2, 1 };
    unsigned i;

    printf("%s test\n", __FUNCTION__);

    /* Add three entries to the list */
    list_dump(head);
    list_add(&test1.list, head);
    list_dump(head);
    list_add(&test2.list, head);
    list_dump(head);
    list_add(&test3.list, head);
    list_dump(head);

    /* Verify the container pointers returned by list_for_each_entry() */
    i = 0;
    list_for_each_entry(entry, struct test_data, head, list) {
        if (entry->num != expected[i]) {
            printf("Expected value:%d, Observed value: %d\n",
                    expected[i], entry->num);
            return EFAIL;
        }
        i++;
    }

    /* Delete the entries */
    list_del(&test1.list, head);
    list_del(&test2.list, head);
    list_del(&test3.list, head);

    return ENO_ERROR;
}

/**
 * @brief Test list_for_each()
 *
 * 1. Add three entries to the list.
 * 2. Fetch each container structure using list_for_each_entry().
 * 3. Verify the data stored in the container structure (thereby
 *    verifying that the container structure are correctly
 *    returned).
 */
static int ll_test_list_for_each(struct list_head *head)
{
    struct test_data test1;
    struct test_data test2;
    struct test_data test3;
    struct list_head *pos;
    /* Expected struct list_head pointers */
    struct list_head *expected[] = { &test3.list, &test2.list, &test1.list };
    unsigned i;

    printf("%s test\n", __FUNCTION__);

    /* Add three entries to the list */
    list_dump(head);
    list_add(&test1.list, head);
    list_dump(head);
    list_add(&test2.list, head);
    list_dump(head);
    list_add(&test3.list, head);
    list_dump(head);

    /* Verify the list_head pointers returned by list_for_each() */
    i = 0;
    list_for_each(pos, head) {
        if (pos != expected[i]) {
            printf("Expected addr:%p, Observed addr: %p\n", expected[i], pos);
            return EFAIL;
        }
        i++;
    }

    /* Delete the entries */
    list_del(&test1.list, head);
    list_del(&test2.list, head);
    list_del(&test3.list, head);

    return ENO_ERROR;
}

/**
 * @brief Top level linked library test function
 */
static int ll_test(void)
{
    static LIST_HEAD(test);
    int err = EFAIL;

    /* Test empty list */
    TEST_AND_EXIT_ON_FAIL("list empty", list_empty(&test) == 1);

    /* Add an entry using list_add to the list and ensure that the
     * new entry is correctly hooked up to the list. Then delete
     * the entry using list_del and verify the list of empty.
     */
    TEST_AND_EXIT_ON_FAIL("list_add_del", ll_test_add_del_entry(&test) == ENO_ERROR);

    /* Add three entries and delete the entires and verify the list
     * structure at each stage.
     */
    TEST_AND_EXIT_ON_FAIL("list_add_del_multiple", ll_test_add_del_entries(&test) == ENO_ERROR);

    /* Add three entries using list_add_tail() and verify that the list
     * is set up correctly.
     */
    TEST_AND_EXIT_ON_FAIL("list_add_tail", ll_test_add_tail(&test) == ENO_ERROR);

    /* Test list_for_each_entry() */
    TEST_AND_EXIT_ON_FAIL("list_for_each_entry", ll_test_list_for_each_entry(&test) == ENO_ERROR);

    /* Test list_for_each() */
    TEST_AND_EXIT_ON_FAIL("list_for_each", ll_test_list_for_each(&test) == ENO_ERROR);

    return ENO_ERROR;
}

/**
 */
int main(void)
{
    printf("Testing linked list library\n");
    if (ll_test() != ENO_ERROR)
        printf("Linked list library test failed\n");
    else
        printf("Linked list library test passed\n");

    return 0;
}

#endif

