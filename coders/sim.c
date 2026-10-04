/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/04 11:49:50 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/04 12:59:33 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_sim(t_simulation *sim)
{
	int	i;

	sim->start_time = get_timestamp_ms();
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

void	cleanup_sim(t_simulation *sim)
{
	free(sim->coders);
	sim->coders = NULL;
}
