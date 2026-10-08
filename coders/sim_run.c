/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_run.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 12:31:18 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/08 18:48:40 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

void	log_state(t_simulation *sim, int id, const char *message)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lli %i %s\n", get_elapsed_ms(sim->start_time), id, message);
	pthread_mutex_unlock(&sim->log_mutex);
}
