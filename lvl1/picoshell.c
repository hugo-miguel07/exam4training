#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int	picoshell(char **cmds[])
{
	int		fd[2];
	int		prev_fd;
	int		pid;
	int		i;
	int		have_pipe;
	int		status;

	prev_fd = -1; /* no previous pipe read end */
	i = 0;
	while (cmds[i])
	{
		have_pipe = (cmds[i + 1] != NULL);
		if (have_pipe)
		{
			if (pipe(fd) == -1)
			{
				if (prev_fd != -1)
					close(prev_fd);
				return (1);
			}
		}
		pid = fork();
		if (pid == -1)
		{
			/* cleanup: close newly created pipe ends if any, and prev_fd */
			if (have_pipe)
			{
				close(fd[0]);
				close(fd[1]);
			}
			if (prev_fd != -1)
				close(prev_fd);
			return (1);
		}
		if (pid == 0)
		{
			/* child */
			if (prev_fd != -1)
			{
				if (dup2(prev_fd, STDIN_FILENO) == -1)
				{
					if (have_pipe)
					{
						close(fd[0]);
						close(fd[1]);
					}
					close(prev_fd);
					exit(1);
				}
				close(prev_fd);
			}
			if (have_pipe)
			{
				close(fd[0]); /* child doesn't read from new pipe */
				if (dup2(fd[1], STDOUT_FILENO) == -1)
				{
					close(fd[1]);
					exit(1);
				}
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			/* execvp failed */
			exit(1);
		}
		/* parent */
		if (prev_fd != -1)
			close(prev_fd);
		if (have_pipe)
		{
			close(fd[1]); /* parent no longer needs write end */
			prev_fd = fd[0]; /* keep read end for next command */
		}
		else
			prev_fd = -1;
		i++;
	}
	/* wait for all children */
	while (wait(&status) > 0)
		;
	return (0);

}
