/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 22:07:09 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 22:23:47 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

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

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALARM, &sa, NULL) < 0)
        return (-1);
    int status = 0;
    pid = fork();
    if (pid < 0)
        return (-1);
    if (pid == 0)
    {
        alarm(timeout);
        f();
        exit(0);
    }
    alarm(time_out);
    if (waitpid(pid, &status, 0) < 0)
    {
        if (errno == EINTR && time_out == 1)
        {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            if (verbose)
                printf("Bad function: timed out after <%ld> seconds\n", timeout);
            return (0);
        }
    }
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
				printf("Nice function!\n");
			return (1);
        }
    }
    if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        if (sig == SIGALRM)
        {
            if (verbose)
			    printf("Bad function: timed out after <%ld> seconds\n", timeout);
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
