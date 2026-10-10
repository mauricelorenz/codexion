/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 12:31:18 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/10 11:13:07 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*run_monitor(void *arg);
static int	check_coders(t_simulation *sim, long long *earliest_deadline);

int	run_sim(t_simulation *sim)
{
	int	i;
	int	j;

	if (pthread_create(&sim->monitor, NULL, run_monitor, sim))
		return (4);
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (pthread_create
			(&sim->coders[i].thread, NULL, run_coder, &sim->coders[i]))
			break ;
		i++;
	}
	j = 0;
	while (j < i)
		pthread_join(sim->coders[j++].thread, NULL);
	pthread_mutex_lock(&sim->state_mutex);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_mutex);
	pthread_join(sim->monitor, NULL);
	if (i != sim->args.number_of_coders)
		return (4);
	return (0);
}

static void	*run_monitor(void *arg)
{
	t_simulation	*sim;
	long long		earliest_deadline;
	struct timespec	ts;
	int				i;

	sim = (t_simulation *)arg;
	pthread_mutex_lock(&sim->state_mutex);
	while (!sim->stop)
	{
		if (check_coders(sim, &earliest_deadline))
			break ;
		ts = get_ts(earliest_deadline);
		pthread_cond_timedwait(&sim->state_cond, &sim->state_mutex, &ts);
	}
	pthread_cond_broadcast(&sim->state_cond);
	pthread_mutex_unlock(&sim->state_mutex);
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
	return (NULL);
}

static int	check_coders(t_simulation *sim, long long *earliest_deadline)
{
	int				i;
	long long		now;
	long long		coder_deadline;

	i = -1;
	now = get_timestamp_ms();
	*earliest_deadline = LLONG_MAX;
	while (++i < sim->args.number_of_coders)
	{
		if (sim->coders[i].compiles_done
			>= sim->args.number_of_compiles_required)
			continue ;
		coder_deadline = sim->coders[i].last_compile_start
			+ sim->args.time_to_burnout;
		if (now >= coder_deadline)
		{
			log_state(sim, sim->coders[i].id, "burned out");
			sim->stop = 1;
			return (1);
		}
		if (coder_deadline < *earliest_deadline)
			*earliest_deadline = coder_deadline;
	}
	return (*earliest_deadline == LLONG_MAX);
}

void	log_state(t_simulation *sim, int id, const char *message)
{
	pthread_mutex_lock(&sim->log_mutex);
	if (sim->log_stopped && strcmp(message, "burned out"))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	printf("%lli %i %s\n", get_elapsed_ms(sim->start_time), id, message);
	if (!strcmp(message, "burned out"))
		sim->log_stopped = 1;
	pthread_mutex_unlock(&sim->log_mutex);
}
