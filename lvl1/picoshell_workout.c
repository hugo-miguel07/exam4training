/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell_workout.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htavares <htavares@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 16:59:13 by htavares          #+#    #+#             */
/*   Updated: 2026/05/13 17:36:41 by htavares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int	picoshell(char **cmds[])
{
	int	fd[2];
	int prev_fd;
	int	pid;
	int	i;

	prev_fd = -1;
	i = 0;
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (1);
		pid = fork();
		if (pid == -1)
		{
			close(fd[0]);
			close(fd[1]);
			return (1);
		}
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				if (dup2(prev_fd, 0) == -1)
				{
					close(fd[0]);
					close(fd[1]);
					exit(1);
				}
				close(prev_fd);
			}
			if (cmds[i + 1])
			{
				close(fd[0]);
				if (dup2(fd[1], 1) == -1)
				{
					close(fd[1]);
					exit(1);
				}
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (prev_fd != -1)
			close(prev_fd);
		if (cmds[i + 1])
		{
			close(fd[1]);
			prev_fd = fd[0];
			
		}
		else
			prev_fd = -1;
		i++;
	}
	whie (wait(NULL) > 0)
		;
	return (0);
}
