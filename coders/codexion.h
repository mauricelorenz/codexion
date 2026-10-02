/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:25:36 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/02 13:00:35 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_args
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_args;

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_simulation	*sim;
}	t_coder;

struct s_simulation
{
	t_args		args;
	long long	start_time;
	t_coder		*coders;
};

int			parse_args(int argc, char **argv, t_args *args);
long long	get_timestamp_ms(void);

#endif
