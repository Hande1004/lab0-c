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
    int n = (q_size(head) - 1) / 2;
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


struct list_head *merge(struct list_head *l1,
                        struct list_head *l2,
                        bool descend)
{
    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);
    struct list_head *tail = &tmp;
    if (!descend) {
        while (l1 && l2) {
            const element_t *node1 = list_entry(l1, element_t, list);
            const element_t *node2 = list_entry(l2, element_t, list);
            if (strcmp(node1->value, node2->value) <= 0) {
                tail->next = l1;
                l1->prev = tail;
                tail = tail->next;
                l1 = l1->next;
            } else {
                tail->next = l2;
                l2->prev = tail;
                tail = tail->next;
                l2 = l2->next;
            }
        }
    } else {
        while (l1 && l2) {
            const element_t *node1 = list_entry(l1, element_t, list);
            const element_t *node2 = list_entry(l2, element_t, list);
            if (strcmp(node1->value, node2->value) >= 0) {
                tail->next = l1;
                l1->prev = tail;
                tail = tail->next;
                l1 = l1->next;
            } else {
                l2->prev = tail;
                tail->next = l2;
                tail = tail->next;
                l2 = l2->next;
            }
        }
    }
    if (l1) {
        tail->next = l1;
        l1->prev = tail;
    }
    if (l2) {
        tail->next = l2;
        l2->prev = tail;
    }
    return tmp.next;
}
struct list_head *mergeSortList(struct list_head *node, bool descend)
{
    if (!node || !node->next)
        return node;
    struct list_head *fast = node->next;
    struct list_head *slow = node;
    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    if (fast) {
        fast->prev = NULL;
    }

    // sort each list
    struct list_head *l1 = mergeSortList(node, descend);
    struct list_head *l2 = mergeSortList(fast, descend);
    return merge(l1, l2, descend);
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *first_node = head->next;
    struct list_head *last_node = head->prev;
    first_node->prev = NULL;
    last_node->next = NULL;
    struct list_head *merge_pos = mergeSortList(first_node, descend);
    head->next = merge_pos;
    merge_pos->prev = head;
    if (!merge_pos) {
        INIT_LIST_HEAD(head);
        return;
    }
    while (merge_pos->next) {
        merge_pos = merge_pos->next;
    }
    merge_pos->next = head;
    head->prev = merge_pos;
}



// struct list_head *merge(struct list_head *l1, struct list_head *l2, bool
// descend) {
//     struct list_head dummy;
//     INIT_LIST_HEAD(&dummy);
//     struct list_head *tail = &dummy;

//     while (l1 && l2) {
//         element_t *e1 = list_entry(l1, element_t, list);
//         element_t *e2 = list_entry(l2, element_t, list);
//         int cmp = strcmp(e1->value, e2->value);

//         bool take_from_l1 = (!descend) ? (cmp <= 0) : (cmp >= 0);
//         if (take_from_l1) {
//             struct list_head *next = l1->next;

//             l1->next = NULL;
//             l1->prev = tail;
//             tail->next = l1;
//             tail = l1;
//             l1 = next;
//         } else {
//             struct list_head *next = l2->next;
//             l2->next = NULL;
//             l2->prev = tail;
//             tail->next = l2;
//             tail = l2;
//             l2 = next;
//         }
//     }
//     if (l1) {
//         tail->next = l1;
//         l1->prev = tail;
//     }
//     if (l2) {
//         tail->next = l2;
//         l2->prev = tail;
//     }
//     return dummy.next;
// }

// struct list_head *mergeSortList(struct list_head *head, bool descend) {
//     if (!head || !head->next)
//         return head;

//     struct list_head *slow = head;
//     struct list_head *fast = head->next;
//     while (fast && fast->next) {
//         slow = slow->next;
//         fast = fast->next->next;
//     }

//     struct list_head *mid = slow->next;
//     slow->next = NULL;
//     if (mid)
//         mid->prev = NULL;
//     struct list_head *left = mergeSortList(head, descend);
//     struct list_head *right = mergeSortList(mid, descend);
//     return merge(left, right, descend);
// }


// void q_sort(struct list_head *head, bool descend)
// {
//     if (!head || list_empty(head) || list_is_singular(head))
//         return;


//     struct list_head *first = head->next;
//     struct list_head *last = head->prev;
//     first->prev = NULL;
//     last->next = NULL;

//     struct list_head *sorted = mergeSortList(first, descend);


//     head->next = sorted;
//     sorted->prev = head;

//     struct list_head *p = sorted;
//     while (p->next)
//         p = p->next;
//     head->prev = p;
//     p->next = head;
// }



// void q_sort(struct list_head *head, bool descend)
// {
//     if (!head || list_empty(head) || list_is_singular(head))return;
//     struct list_head *pos = head->next,*safe = pos->next;
//     int count = 1;
//     for(;safe != head;){
//         struct list_head *tmp = safe->next;
//         element_t *node = list_entry(safe,element_t,list);
//         element_t *node_cmp = list_entry(pos,element_t,list);
//         if (!descend){
//             for(int i = 0;i < count && strcmp(node->value,node_cmp->value) >
//             0;pos = pos->next){
//                 node_cmp = list_entry(pos,element_t,list);
//                 i++;
//             }
//             list_move(safe,pos->prev);
//             pos = head->next;
//             count++;
//         }else{
//             for(int i = 0;i < count && strcmp(node->value,node_cmp->value) <
//             0;pos = pos->next){
//                 node_cmp = list_entry(pos,element_t,list);
//                 i++;
//             }
//             list_move(safe,pos->prev);
//             pos = head->next;
//             count++;
//         }
//         safe = tmp;
//     }
// }

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *pos, *safe;
    element_t *node = list_entry(head->prev, element_t, list);
    const char *mini_value = node->value;
    for (pos = head->prev, safe = pos->prev; pos != head;
         pos = safe, safe = pos->prev) {
        node = list_entry(pos, element_t, list);
        if (strcmp(node->value, mini_value) > 0) {
            list_del(pos);
            q_release_element(node);
        } else {
            mini_value = node->value;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *pos, *safe;
    element_t *node = list_entry(head->prev, element_t, list);
    const char *max_value = node->value;
    for (pos = head->prev, safe = pos->prev; pos != head;
         pos = safe, safe = pos->prev) {
        node = list_entry(pos, element_t, list);
        if (strcmp(node->value, max_value) < 0) {
            list_del(pos);
            q_release_element(node);
        } else {
            max_value = node->value;
        }
    }
    return q_size(head);
}

#define __LIST_HAVE_TYPEOF
/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *atx = NULL,
                   *first = list_first_entry(head, queue_contex_t, chain);
    list_for_each_entry (atx, head, chain) {
        if (atx == first)
            continue;
        if (atx && atx->q) {
            list_splice_tail_init(atx->q, first->q);
            first->size += atx->size;
            atx->size = 0;
        }
    }
    q_sort(first->q, descend);
    return first->size;
}
