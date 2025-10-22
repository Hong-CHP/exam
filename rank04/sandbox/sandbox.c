/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 16:52:31 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 17:21:37 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int time_out = 0;

void handler(int signum)
{
	(void)signum;
	time_out = 1;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t pid;
	struct sigaction sa;
	int status = 0;
	
	sa.sa_handler = handler;
	sa.sa_mask = (sigset_t){0};
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (-1);
	if (pid == 0)
	{
		alarm(timeout);
		f();
		exit(0);
	}
	alarm(timeout);
	if (waitpid(pid, &status, 0) < 0)
	{
		if (errno == EINTR && time_out == 1)
		{
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			if (verbose)
				printf("Bad function: timed out after <%d> seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
	if (WIFEXITED(status))
	{
		int code = WEXITSTATUS(status);
		if (code != 0)
		{
			if (verbose)
				printf("Bad function: exited with code <%d>\n", code);
			return (0);
		}
		else
		{
			if (verbose)
				printf("nice function!\n");
			return (1);		
		}
	}
	if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		if (sig == SIGALRM)
		{
			if (verbose)
				printf("Bad function: timed out after <%d> seconds\n", timeout);
			return (0);
		}
		else
		{
			if (verbose)
				printf("Bad function: <%s>\n", strsignal(sig));
			return (0);
		}
	}
	return (-1);
}

void nice_func(void) { }
void bad_exit(void) { exit(42); }
void segv(void) { int *p = 0; *p = 1; }
void infinite(void) { while (1); }

int main(void)
{
	sandbox(nice_func, 1, true);
	sandbox(bad_exit, 1, true);
	sandbox(segv, 1, true);
	sandbox(infinite, 1, true);
}
