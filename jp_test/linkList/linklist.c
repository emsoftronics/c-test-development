


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "linklist.h"


int sll_addToList(slist_t *list, void *data, unsigned int data_length, int data_type)
{
    struct sll_node *tmp = NULL;
    void *mem = NULL;
    if ((!data) || (!list)) return -EINVAL;
    mem = malloc(data_length + sizeof(struct sll_node));
    if (!mem) return -ENOMEM;
    memcpy(mem, data, data_length);
    tmp = (struct sll_node *)((char*)mem + data_length);
    tmp->item_length = data_length;
    tmp->item_type = data_type;
    tmp->next = NULL;
    pthread_mutex_lock(&list->lock);
    if (list->first_entry == NULL) list->first_entry = tmp;

    if (list->last_entry == NULL) list->last_entry = tmp;
    else {
        list->last_entry->next = tmp;
        list->last_entry = tmp;
    }
    list->item_count++;
    pthread_mutex_unlock(&list->lock);
    return 0;
}

int sll_addAtHead(slist_t *list, void *data, unsigned int data_length, int data_type)
{
    int ret = -1;
    slist_t temp = SLLIST_INITIALIZER;
    if ((!list) || (!data)) return -ENOMEM;
    ret = sll_addToList(&temp, data, data_length, data_type);
    if (ret < 0) return ret;

    pthread_mutex_lock(&list->lock);
    temp.first_entry->next = list->first_entry;
    list->first_entry = temp.first_entry;
    list->item_count++;
    pthread_mutex_unlock(&list->lock);
    return 0;
}

int sll_addAtTail(slist_t *list, void *data, unsigned int data_length, int data_type)
{
    return sll_addToList(list, data, data_length, data_type);
}

int sll_removeFromHead(slist_t *list, void *outbuf, int *data_type)
{
    void *mem = NULL;
    int length = 0;
    if (!list) return -EINVAL;
    if (!list->first_entry) return -ENOENT;
    pthread_mutex_lock(&list->lock);
    mem = ((char*)list->first_entry) - list->first_entry->item_length;
    if (outbuf) {
        memcpy(outbuf, mem, list->first_entry->item_length);
        length = list->first_entry->item_length;
    }
    if (data_type) {
        *data_type = list->first_entry->item_type;
    }
    list->first_entry = list->first_entry->next;
    list->item_count--;
    pthread_mutex_unlock(&list->lock);
    free(mem);
    return length;
}

int sll_removeFromTail(slist_t *list, void *outbuf, int *data_type)
{
    void *mem = NULL;
    int length = 0;
    struct sll_node *tmp = NULL;
    if (!list) return -EINVAL;
    if (!list->first_entry) return -ENOENT;
    pthread_mutex_lock(&list->lock);
    for(list->last_entry = list->first_entry;
                (list->last_entry->next != NULL) && (list->last_entry->next->next != NULL);
                list->last_entry = list->last_entry->next);

    if (!list->last_entry->next) {
        tmp = list->last_entry;
        list->last_entry = list->first_entry = NULL;
    }
    else {
        tmp = list->last_entry->next;
        list->last_entry->next = NULL;
    }
    mem = ((char*)tmp) - tmp->item_length;
    if (outbuf) {
        memcpy(outbuf, mem, tmp->item_length);
        length = tmp->item_length;
    }
    if (data_type) {
        *data_type = tmp->item_type;
    }
    list->item_count--;
    pthread_mutex_unlock(&list->lock);
    free(mem);
    return length;
}

int sll_getListItemCount(slist_t *list)
{
    int ret = 0;
    pthread_mutex_lock(&list->lock);
    ret = list->item_count;
    pthread_mutex_unlock(&list->lock);
    return ret;
}

static void sll_swap(slist_t *list, struct sll_node *prev)
{
    struct sll_node *cur = NULL;
    if (!list || !list->first_entry) return;
    if ((!prev) && list->first_entry->next) {
        cur = list->first_entry;
        list->first_entry = cur->next;
        cur->next = cur->next->next;
        list->first_entry->next = cur;
    }
    else if (prev && prev->next && prev->next->next){
        cur = prev->next;
        prev->next = cur->next;
        cur->next = prev->next->next;
        prev->next->next = cur;
    }

    if (cur && !cur->next) list->last_entry = cur;
}

int sll_sortList(slist_t *list, compare_t compare, int reverse)
{
    struct sll_node *cur = NULL;
    struct sll_node *prev = NULL;
    sliteminfo_t cinfo, ninfo;
    int swap = 0, count, comp, i;
    if (!list) return -EINVAL;
    if (!list->first_entry) return -ENOENT;
    pthread_mutex_lock(&list->lock);

    for(count = 0; count < list->item_count; count++)
    for(cur = list->first_entry, prev = NULL, i = 0;
            (i < list->item_count - count) && (cur->next != NULL);
            cur = cur->next, i++) {
        swap = 0;
        cinfo = *((sliteminfo_t *)cur);
        ninfo = *((sliteminfo_t *)cur->next);
        cinfo.item = (char *)cur - cur->item_length;
        ninfo.item = (char *)(cur->next) - cur->next->item_length;
        if (compare) {
            comp = compare(&cinfo, &ninfo);
            if (((comp > 0) && (!reverse)) || ((comp < 0) && reverse)) swap = 1;
        }
        else {
            if (((*((unsigned int *)cinfo.item) > *((unsigned int *)ninfo.item)) && !reverse)
                    || ((*((unsigned int *)cinfo.item) < *((unsigned int *)ninfo.item)) && reverse)) swap = 1;
        }

        if (swap) {
            sll_swap(list, prev);
            if (!prev) cur = list->first_entry;
            else cur = prev->next;
        }
        prev = cur;
    }
    pthread_mutex_unlock(&list->lock);
    return 0;
}

int sll_getListItem(sllist_traverse_t cmd, slist_t *list,void *outbuf, int *data_type)
{
    static struct sll_node *tmp = NULL;
    static struct sll_node *head = NULL;
    static int count = 0;
    struct sll_node *t = NULL;
    void *mem = NULL;
    int ret = 0;


    if (!list) return -EINVAL;
    if (!list->first_entry) return -ENOENT;

    pthread_mutex_lock(&list->lock);
    if ((head != list->first_entry) || (count != list->item_count)) {
        tmp = head = list->first_entry;
        count = list->item_count;
    }

    switch(cmd) {
        case ITEM_BEGIN:
            t = tmp = list->first_entry;
            break;
        case ITEM_NEXT:
            if (tmp) tmp = tmp->next;
            t = tmp;
            break;
        case ITEM_END:
            t = tmp = list->last_entry;
            break;
        case ITEM_FIRST:
            t = list->first_entry;
            break;
        case ITEM_LAST:
            t = list->last_entry;
            break;
        default: ret = -EINVAL;
    }

    if(ret < 0);
    else if (!t) ret =  -EEXIST;
    else {
        if (outbuf) {
            mem = ((char *)t) - t->item_length;
            memcpy(outbuf, mem, t->item_length);
        }

        if (data_type) *data_type = t->item_type;
        ret = t->item_length;
    }
    pthread_mutex_unlock(&list->lock);
    return ret;
}

int sll_push(slstack_t *stack, void *data, unsigned int data_length, int data_type)
{
    if ((!stack) || (!data)) return -EINVAL;
    if (stack->list.item_count >= stack->stack_limit) return -EOVERFLOW;
    return sll_addAtHead(&stack->list, data, data_length, data_type);
}

int sll_pop(slstack_t *stack, void *outbuf, int *data_type)
{
    if (!stack) return -EINVAL;
    return sll_removeFromHead(&stack->list, outbuf, data_type);
}

int sll_enqueue(slqueue_t *queue, void *data, unsigned int data_length, int data_type)
{
    if ((!queue) || (!data)) return -EINVAL;
    if (queue->list.item_count >= queue->queue_limit) return -EOVERFLOW;
    return sll_addAtTail(&queue->list, data, data_length, data_type);
}

int sll_dequeue(slqueue_t *queue, void *outbuf, int *data_type)
{
    if (!queue) return -EINVAL;
    return sll_removeFromHead(&queue->list, outbuf, data_type);
}

