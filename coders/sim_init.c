/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 11:49:50 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/07 12:34:52 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_simulation *sim);
static void	cleanup_dongles(t_simulation *sim, int i);

int	init_sim(t_simulation *sim)
{
	int	i;
	int	result;

	sim->start_time = get_timestamp_ms();
	pthread_mutex_init(&sim->log_mutex, NULL);
	sim->coders = malloc(sim->args.number_of_coders * sizeof(t_coder));
	if (!sim->coders)
		return (3);
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].sim = sim;
		i++;
	}
	result = init_dongles(sim);
	if (result)
		return (result);
	return (0);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	sim->dongles = malloc(sim->args.number_of_coders * sizeof(t_dongle));
	if (!sim->dongles)
		return (3);
	memset(sim->dongles, 0, sim->args.number_of_coders * sizeof(t_dongle));
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (pthread_cond_init(&sim->dongles[i].cond, NULL))
			return (cleanup_dongles(sim, i), 4);
		i++;
	}
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		sim->dongles[i].heap.scheduler = sim->args.scheduler;
		i++;
	}
	return (0);
}

static void	cleanup_dongles(t_simulation *sim, int i)
{
	int	j;

	j = 0;
	while (j < i)
	{
		pthread_cond_destroy(&sim->dongles[j].cond);
		j++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

void	cleanup_sim(t_simulation *sim)
{
	int	i;

	i = 0;
	if (sim->dongles)
	{
		while (i < sim->args.number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			pthread_cond_destroy(&sim->dongles[i].cond);
			i++;
		}
	}
	free(sim->dongles);
	sim->dongles = NULL;
	free(sim->coders);
	sim->coders = NULL;
	pthread_mutex_destroy(&sim->log_mutex);
}
