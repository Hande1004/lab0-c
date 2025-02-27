#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    struct list_head *pos, *safe;
    list_for_each_safe (pos, safe, head) {
        element_t *node = list_entry(pos, element_t, list);
        list_del(pos);
        q_release_element(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp && bufsize > 0 && node->value) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp && bufsize > 0 && node->value) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *pos;
    int n = 0;
    list_for_each (pos, head) {
        n++;
    }
    return n;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    int n = (q_size(head) - 1) >> 1;
    struct list_head *pos = head->next;
    for (int i = 0; i < n; i++) {
        pos = pos->next;
    }
    element_t *node = list_entry(pos, element_t, list);
    list_del(pos);
    q_release_element(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head *pos, *safe;
    bool dup = false;
    list_for_each_safe (pos, safe, head) {
        element_t *node = list_entry(pos, element_t, list);
        if (safe != head) {
            const element_t *node_next = list_entry(safe, element_t, list);
            if (strcmp(node->value, node_next->value) == 0) {
                dup = true;
                list_del(pos);
                q_release_element(node);
                continue;
            }
            if (dup) {
                list_del(pos);
                q_release_element(node);
                dup = false;
            }
            continue;
        }
        if (dup) {
            list_del(pos);
            q_release_element(node);
        }
        break;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *pos, *safe;
    for (pos = head->next, safe = pos->next; pos != head && safe != head;
         pos = pos->next, safe = pos->next) {
        list_del(pos);
        list_add(pos, safe);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *pos, *safe;
    list_for_each_safe (pos, safe, head) {
        struct list_head *next = pos->next;
        struct list_head *prev = pos->prev;
        pos->prev = next;
        pos->next = prev;
    }
    struct list_head *h_next = head->next;
    head->next = head->prev;
    head->prev = h_next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k == 1)
        return;
    struct list_head *pos = head->next, *safe, *first, *first_prev;
    int num = q_size(head);
    for (int i = 0; i < num / k; i++) {
        struct list_head **first_next = NULL;
        int m = 0;
        for (safe = pos->next; m < k; pos = safe, safe = pos->next, m++) {
            struct list_head *next = pos->next;
            struct list_head *prev = pos->prev;
            if (m == 0) {
                first = pos;
                first_prev = prev;
                first_next = &pos->next;
                pos->prev = next;
                continue;
            }
            if (0 < m && m < k - 1) {
                pos->prev = next;
                pos->next = prev;
                continue;
            }
            if (m == k - 1) {
                pos->next = prev;
                pos->prev = first_prev;
                *first_next = next;
                first_prev->next = pos;
                next->prev = first;
                pos = next;
                break;
            }
        }
    }
}


