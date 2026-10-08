/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 18:47:55 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/08 19:24:40 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	run_compile(t_coder *coder);
static void	get_dongle_order(int *dongle_array, t_coder *coder);
static int	sleep_coder(t_coder *coder, int duration);

void	*run_coder(void *arg)
{
	t_coder			*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		if (run_compile(coder))
			break ;
		log_state(coder->sim, coder->id, "is debugging");
		if (sleep_coder(coder, coder->sim->args.time_to_debug))
			break ;
		log_state(coder->sim, coder->id, "is refactoring");
		if (sleep_coder(coder, coder->sim->args.time_to_refactor))
			break ;
	}
	return (NULL);
}

static int	run_compile(t_coder *coder)
{
	int	dongle_array[2];

	get_dongle_order(dongle_array, coder);
	if (take_dongle(&coder->sim->dongles[dongle_array[0]], coder))
		return (1);
	log_state(coder->sim, coder->id, "has taken a dongle");
	if (take_dongle(&coder->sim->dongles[dongle_array[1]], coder))
		return (1);
	log_state(coder->sim, coder->id, "has taken a dongle");
	pthread_mutex_lock(&coder->sim->state_mutex);
	coder->last_compile_start = get_timestamp_ms();
	pthread_cond_broadcast(&coder->sim->state_cond);
	pthread_mutex_unlock(&coder->sim->state_mutex);
	log_state(coder->sim, coder->id, "is compiling");
	if (sleep_coder(coder, coder->sim->args.time_to_compile))
		return (1);
	release_dongle(&coder->sim->dongles[dongle_array[0]], coder);
	release_dongle(&coder->sim->dongles[dongle_array[1]], coder);
	pthread_mutex_lock(&coder->sim->state_mutex);
	coder->compiles_done++;
	pthread_cond_broadcast(&coder->sim->state_cond);
	pthread_mutex_unlock(&coder->sim->state_mutex);
	return (0);
}

static void	get_dongle_order(int *dongle_array, t_coder *coder)
{
	if (coder->id % 2)
	{
		dongle_array[0] = coder->id - 1;
		dongle_array[1] = coder->id % coder->sim->args.number_of_coders;
	}
	else
	{
		dongle_array[0] = coder->id % coder->sim->args.number_of_coders;
		dongle_array[1] = coder->id - 1;
	}
}

static int	sleep_coder(t_coder *coder, int duration)
{
	long long		deadline;
	struct timespec	ts;

	deadline = get_timestamp_ms() + duration;
	ts = get_ts(deadline);
	pthread_mutex_lock(&coder->sim->state_mutex);
	while (!coder->sim->stop && get_timestamp_ms() < deadline)
		pthread_cond_timedwait(&coder->sim->state_cond,
			&coder->sim->state_mutex, &ts);
	pthread_mutex_unlock(&coder->sim->state_mutex);
	return (coder->sim->stop);
}
