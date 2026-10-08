/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlorenz <mlorenz@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 11:42:01 by mlorenz           #+#    #+#             */
/*   Updated: 2026/10/08 17:16:17 by mlorenz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long long	get_elapsed_ms(long long start_time)
{
	return (get_timestamp_ms() - start_time);
}

struct timespec	get_ts(long long timestamp)
{
	struct timespec	ts;

	ts.tv_sec = timestamp / 1000;
	ts.tv_nsec = timestamp % 1000 * 1000000;
	return (ts);
}
