
#include <stdio.h>
#include <string.h>
#include "adx_alloc.h"

typedef struct {
    adx_list_t list;
    void *block;
} adx_pool_block_t;

adx_pool_t *adx_pool_create()
{
    adx_pool_t *pool = (adx_pool_t *)je_malloc(sizeof(adx_pool_t));
    if (!pool) return NULL;

    adx_list_init(&pool->head);
    pool->size = 0;
    return pool;
}

void *adx_alloc(adx_pool_t *pool, size_t size)
{
    char *p = (char *)je_malloc(sizeof(adx_pool_block_t) + size);
    if (!p) return NULL;

    adx_pool_block_t *b = (adx_pool_block_t *)p;
    b->block = p + sizeof(adx_pool_block_t);

    pool ->size += size;
    memset(b->block, 0, size);
    adx_list_add(&pool->head, &b->list);
    // fprintf(stdout, "new=%p\n", b->block);
    return b->block;
}

size_t adx_poll_size(adx_pool_t *pool)
{
    return pool ? pool->size : 0;
}

void adx_pool_free(adx_pool_t *pool)
{
    if (!pool) return;

    adx_list_t *p = NULL;
    adx_list_t *head = &pool->head;
    for (p = head->next; p != head; ) {
        adx_pool_block_t *b = (adx_pool_block_t *)p;
        // fprintf(stdout, "free=%p\n", b->block);
        p = p->next;
        je_free(b);
    }

    je_free(pool);
}



