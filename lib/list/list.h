/**
 * @file  list.h
 *
 * @brief Linked List Library
 *
 * Linked list is implemented as singly linked circular list
 * similar to the list implementation in the linux kernel (which
 * implements the list as a doubly linked list). Single linked
 * list is chosen here to reduce the memory footprint of the list
 * structure.
 *
 * Usage:
 * Create a list named \a test as follows:
 *
 * #include "list.h"
 * static LIST_HEAD(test);
 *
 * To add a data structure into the list, first define struct
 * list_head as a member of the data structure as shown below:
 *
 * struct test_data {
 *     struct list_head list;
 *     <add other data items in the structure>
 * };
 *
 * Note that the \a list member need not be the first element
 * of the structure.
 *
 * To add this data structure to the head of \a test list, do this:
 *
 * struct test_data test_data;
 * list_add(&test_data.list, test);
 *
 * To add the data structure to the tail of \a test list, do this:
 *
 * struct test_data test_data;
 * list_add_tail(&test_data.list, test);
 *
 * To delete the data structure from the list \a test:
 * list_del(&test_data.list, test);
 *
 * Convenience macros to iterate through the list are also defined
 * in this header file.
 *
 * TODO: Implement doubly linked list API if the performance of
 * hit of list_del() is unacceptable.
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "common.h"

/**
 * @brief Define and initialize a list head
 *
 * The next pointer of the head is initialized to point to itself (to
 * indicate an empty list).
 */
#define LIST_HEAD_INIT(name)    { &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

/**
 * @brief Iterate through the list
 *
 * @param pos  &struct list_head to be used as the iterator.
 * @param head List head pointer.
 */
#define list_for_each(pos, head) \
            for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief Get the container structure for this entry
 *
 * @param ptr    &struct list_head pointer that is embedded in the containing
 *               structure.
 * @param type   Type of the container structure that this entry is embedded in.
 * @param member The name of the list_head member within the container
 *               structure.
 */
#define list_entry(ptr, type, member) \
            container_of(ptr, type, member)

/**
 * @brief Get the first container structure from the list
 *
 * @param head   The list head pointer.
 * @param type   Type of the container structure.
 * @param member The name of the list_head member within the container
 *               structure.
 */
#define list_first_entry(head, type, member) \
            list_entry((head)->next, type, member)

/**
 * @brief Get the next container structure from the list
 *
 * @param pos    Pointer to the container structure (usually obtained from
 *               list_first_entry().
 * @param type   Type of the container structure.
 * @param member The name of the list_head member within the container
 *               structure.
 */
#define list_next_entry(pos, type, member) \
            list_entry((pos)->member.next, type, member)

/**
 * @brief Iterate through the list containers of given type
 *
 * @param pos    Pointer to the container structure used to iterate
 *               through the list.
 * @param head   The list head pointer.
 * @param type   Type of the container structure.
 * @param member The name of the list_head member within the container
 *               structure.
 */
#define list_for_each_entry(pos, type, head, member)         \
            for (pos = list_first_entry(head, type, member); \
                 &pos->member != head;                       \
                 pos = list_next_entry(pos, type, member))

/**
 * @brief Test if the list is empty
 *
 * The list is empty if the head points to itself.
 */
#define list_empty(head)    ((head)->next == head)

/**
 * @brief Add an element at the end of the list
 *
 * @param new  New entry to be added.
 * @param head The list head pointer.
 */
void list_add_tail(struct list_head *new, struct list_head *head);

/**
 * @brief Add an element at the head of the list
 *
 * @param new  New entry to be added.
 * @param head The list head pointer.
 */
void list_add(struct list_head *new, struct list_head *head);

/**
 * @brief Delete an element from the list
 *
 * @param new  New entry to be deleted.
 * @param head The list head pointer.
 */
void list_del(struct list_head *entry, struct list_head *head);

#endif /* __LIST_H__ */

