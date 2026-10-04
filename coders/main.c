/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:25:30 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/04 16:42:09 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	int				result;
	t_simulation	sim;

	memset(&sim, 0, sizeof(sim));
	result = parse_args(argc, argv, &sim.args);
	if (result)
		return (result);
	result = init_sim(&sim);
	if (result)
		return (cleanup_sim(&sim), result);
	result = run_sim(&sim);
	if (result)
		return (cleanup_sim(&sim), result);
	cleanup_sim(&sim);
	return (0);
}
