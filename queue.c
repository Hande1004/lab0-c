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

