/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 16:45:02 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/08 17:26:03 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	can_take(t_dongle *dongle, int coder_id);
static int	is_head(t_dongle *dongle, int coder_id);

int	take_dongle(t_dongle *dongle, t_coder *coder)
{
	t_heap_entry	entry;
	struct timespec	ts;

	entry.coder_id = coder->id;
	entry.deadline = coder->last_compile_start
		+ coder->sim->args.time_to_burnout;
	pthread_mutex_lock(&dongle->mutex);
	push_heap(&dongle->heap, entry);
	while (!can_take(dongle, coder->id))
	{
		ts = get_ts(dongle->available_at);
		if (is_head(dongle, coder->id) && !dongle->taken)
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	dongle->taken = 1;
	pop_heap(&dongle->heap);
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
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

void	release_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->taken = 0;
	dongle->available_at = get_timestamp_ms()
		+ coder->sim->args.dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
