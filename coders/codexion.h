/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:25:36 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/09 21:20:12 by mlorenz          ###   ########.fr       */
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

typedef struct s_simulation	t_simulation;

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

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_simulation	*sim;
	long long		last_compile_start;
	int				compiles_done;
}	t_coder;

typedef struct s_heap_entry
{
	int			coder_id;
	long long	deadline;
	int			seq;
}	t_heap_entry;

typedef struct s_heap
{
	t_heap_entry	entries[2];
	int				size;
	int				seq_next;
	t_scheduler		scheduler;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			heap;
	int				taken;
	long long		available_at;
}	t_dongle;

struct s_simulation
{
	t_args			args;
	long long		start_time;
	t_coder			*coders;
	pthread_mutex_t	log_mutex;
	t_dongle		*dongles;
	pthread_mutex_t	state_mutex;
	pthread_cond_t	state_cond;
	int				stop;
	pthread_t		monitor;
	int				log_stopped;
};

int				parse_args(int argc, char **argv, t_args *args);
long long		get_timestamp_ms(void);
int				init_sim(t_simulation *sim);
void			cleanup_sim(t_simulation *sim);
int				run_sim(t_simulation *sim);
long long		get_elapsed_ms(long long start_time);
void			log_state(t_simulation *sim, int id, const char *message);
void			push_heap(t_heap *heap, t_heap_entry entry);
t_heap_entry	peek_heap(t_heap *heap);
t_heap_entry	pop_heap(t_heap *heap);
int				take_dongle(t_dongle *dongle, t_coder *coder);
void			release_dongle(t_dongle *dongle, t_coder *coder);
struct timespec	get_ts(long long timestamp);
void			*run_coder(void *arg);

#endif
