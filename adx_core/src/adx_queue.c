
#include "adx_queue.h"

void adx_queue_push(adx_list_t *queue, adx_str_t str)
{
	if (adx_empty(str)) return;

	adx_queue_t *node = je_malloc(sizeof(adx_queue_t));
	if (!node) return;

	node->str = str;
	adx_list_add(queue, &node->queue);
}

void adx_queue_push_dup(adx_list_t *queue, adx_str_t str)
{
	return adx_queue_push(queue, adx_strdup(str));
}

adx_str_t adx_queue_pop(adx_list_t *queue)
{
	adx_str_t str = {0};
	adx_queue_t *node = (adx_queue_t *)adx_queue(queue);
	if (node) {
		str = node->str;
		je_free(node);
	}

	return str;
}



