/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 18:33:23 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 18:42:38 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int    picoshell(char **cmds[])
{
	pid_t pid;
	int pipefd[2];
	int prev = -1;
	int status = 0;
	int i = 0;
	
	if (!cmds || !cmds[0] || !cmds[0][0] || !*cmds[0][0])
		return (1);
	while (cmds[i])
	{
		if (cmds[i + 1])
		{
			if (pipe(pipefd) == -1)
				return (1);
		}
		else
		{
			pipefd[0] = -1;
			pipefd[1] = -1;
		}
		pid = fork();
		if (pid < 0)
		{
			if (pipefd[0] != -1)
				close(pipefd[0]);
			if (pipefd[1] != -1)
				close(pipefd[1]);
			if (prev != -1)
				close(prev);
		}
		if (pid == 0)
		{
			if (prev != -1)
			{
				close(pipefd[0]);
				dup2(prev, STDIN_FILENO);
				close(prev);
			}
			if (pipefd[1] != -1)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else
		{
			if (prev != -1)
				close(prev);
			if (cmds[i + 1])
				close(pipefd[1]);
			prev = pipefd[0];
			i++;
		}
	}
	wait(&status);
	return (0);
}