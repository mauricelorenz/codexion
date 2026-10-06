/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 16:05:32 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/06 21:18:43 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_better(t_heap_entry a, t_heap_entry b, t_scheduler scheduler);
static void	swap_entries(t_heap_entry *a, t_heap_entry *b);

void	push_heap(t_heap *heap, t_heap_entry entry)
{
	int	i;
	int	p;

	entry.seq = heap->seq_next;
	heap->entries[heap->size] = entry;
	heap->seq_next++;
	heap->size++;
	i = heap->size - 1;
	while (i > 0)
	{
		p = (i - 1) / 2;
		if (is_better(heap->entries[i], heap->entries[p], heap->scheduler))
		{
			swap_entries(&heap->entries[i], &heap->entries[p]);
			i = p;
		}
		else
			break ;
	}
}

t_heap_entry	peek_heap(t_heap *heap)
{
	return (heap->entries[0]);
}

t_heap_entry	pop_heap(t_heap *heap)
{
	int				i;
	int				c;
	t_heap_entry	entry;

	entry = heap->entries[0];
	heap->entries[0] = heap->entries[heap->size - 1];
	heap->size--;
	i = 0;
	while ((2 * i + 1) < heap->size)
	{
		c = 2 * i + 1;
		if (((2 * i + 2) < heap->size) && is_better(heap->entries[2 * i + 2],
				heap->entries[2 * i + 1], heap->scheduler))
			c = 2 * i + 2;
		if (is_better(heap->entries[c], heap->entries[i], heap->scheduler))
		{
			swap_entries(&heap->entries[c], &heap->entries[i]);
			i = c;
		}
		else
			break ;
	}
	return (entry);
}

static int	is_better(t_heap_entry a, t_heap_entry b, t_scheduler scheduler)
{
	if (scheduler == EDF)
		if (a.deadline != b.deadline)
			return (a.deadline < b.deadline);
	return (a.seq < b.seq);
}

static void	swap_entries(t_heap_entry *a, t_heap_entry *b)
{
	t_heap_entry	c;

	c = *a;
	*a = *b;
	*b = c;
}
