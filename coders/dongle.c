/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 16:45:02 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/07 21:27:15 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	can_take(t_dongle *dongle, int coder_id);
static int	is_head(t_dongle *dongle, int coder_id);

void	take_dongle(t_dongle *dongle, int coder_id, long long deadline)
{
	t_heap_entry	entry;
	struct timespec	ts;

	entry.coder_id = coder_id;
	entry.deadline = deadline;
	pthread_mutex_lock(&dongle->mutex);
	push_heap(&dongle->heap, entry);
	while (!can_take(dongle, coder_id))
	{
		ts.tv_sec = dongle->available_at / 1000;
		ts.tv_nsec = dongle->available_at % 1000 * 1000000;
		if (is_head(dongle, coder_id) && !dongle->taken)
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dongle->taken = 1;
	pop_heap(&dongle->heap);
	pthread_mutex_unlock(&dongle->mutex);
}

static int	can_take(t_dongle *dongle, int coder_id)
{
	return (is_head(dongle, coder_id)
		&& !dongle->taken
		&& get_timestamp_ms() >= dongle->available_at);
}

static int	is_head(t_dongle *dongle, int coder_id)
{
	return (dongle->heap.size > 0
		&& peek_heap(&dongle->heap).coder_id == coder_id);
}

void	release_dongle(t_dongle *dongle, int cooldown)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->taken = 0;
	dongle->available_at = get_timestamp_ms() + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
