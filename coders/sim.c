/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 11:49:50 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/04 18:26:33 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*run_coder(void *arg);

int	init_sim(t_simulation *sim)
{
	int	i;

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
	return (0);
}

int	run_sim(t_simulation *sim)
{
	int	i;
	int	j;

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
	{
		pthread_join(sim->coders[j].thread, NULL);
		j++;
	}
	if (i != sim->args.number_of_coders)
		return (4);
	return (0);
}

static void	*run_coder(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	log_state(coder->sim, coder->id, "has started");
	return (NULL);
}

void	log_state(t_simulation *sim, int id, const char *message)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lli %i %s\n", get_elapsed_ms(sim->start_time), id, message);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	cleanup_sim(t_simulation *sim)
{
	free(sim->coders);
	sim->coders = NULL;
	pthread_mutex_destroy(&sim->log_mutex);
}
