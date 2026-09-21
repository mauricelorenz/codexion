/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 14:09:58 by mlorenz           #+#    #+#             */
/*   Updated: 2026/09/21 15:25:51 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	print_usage(void);

int	parse_args(int argc, char **argv, t_args *args)
{
	if (argc != 9)
		return (print_usage(), 1);
	(void)argv;
	(void)args;
	return (0);
}

static void	print_usage(void)
{
	fprintf(stderr, "usage: ./codexion <number_of_coders> <time_to_burnout> "
		"<time_to_compile> <time_to_debug> <time_to_refactor> "
		"<number_of_compiles_required> <dongle_cooldown> <scheduler>\n");
}
