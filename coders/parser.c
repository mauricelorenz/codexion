/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 14:09:58 by mlorenz           #+#    #+#             */
/*   Updated: 2026/09/22 13:12:32 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	print_error(int error, char *content, char *context);
static int	parse_int(char *str, int *result);
static int	parse_scheduler(char *str, t_scheduler *result);

int	parse_args(int argc, char **argv, t_args *args)
{
	if (argc != 9)
		return (print_error(1, NULL, NULL));
	if (parse_int(argv[1], &args->number_of_coders) || !args->number_of_coders)
		return (print_error(2, argv[1], "number_of_coders (int > 0)"));
	if (parse_int(argv[2], &args->time_to_burnout))
		return (print_error(2, argv[2], "time_to_burnout (int >= 0)"));
	if (parse_int(argv[3], &args->time_to_compile))
		return (print_error(2, argv[3], "time_to_compile (int >= 0)"));
	if (parse_int(argv[4], &args->time_to_debug))
		return (print_error(2, argv[4], "time_to_debug (int >= 0)"));
	if (parse_int(argv[5], &args->time_to_refactor))
		return (print_error(2, argv[5], "time_to_refactor (int >= 0)"));
	if (parse_int(argv[6], &args->number_of_compiles_required))
		return (print_error(2, argv[6],
				"number_of_compiles_required (int >= 0)"));
	if (parse_int(argv[7], &args->dongle_cooldown))
		return (print_error(2, argv[7], "dongle_cooldown (int >= 0)"));
	if (parse_scheduler(argv[8], &args->scheduler))
		return (print_error(2, argv[8], "scheduler (fifo|edf)"));
	return (0);
}

static int	print_error(int error, char *content, char *context)
{
	if (error == 1)
		fprintf(stderr, "usage: ./codexion <number_of_coders> <time_to_burnout>"
			" <time_to_compile> <time_to_debug> <time_to_refactor>"
			" <number_of_compiles_required> <dongle_cooldown> <scheduler>\n");
	else if (error == 2)
		fprintf(stderr, "error: invalid argument '%s' for %s\n", content,
			context);
	return (error);
}

static int	parse_int(char *str, int *result)
{
	int	value;

	if (!*str)
		return (1);
	value = 0;
	while (*str)
	{
		if (*str < '0' || *str > '9' || value > (INT_MAX - (*str - '0')) / 10)
			return (1);
		value = value * 10 + (*str - '0');
		str++;
	}
	*result = value;
	return (0);
}

static int	parse_scheduler(char *str, t_scheduler *result)
{
	if (!strcmp(str, "fifo"))
	{
		*result = FIFO;
		return (0);
	}
	else if (!strcmp(str, "edf"))
	{
		*result = EDF;
		return (0);
	}
	return (1);
}
