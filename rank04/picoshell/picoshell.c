/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 21:42:31 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 22:01:49 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    int pipefd[2];
    int prev = -1;
    pid_t pid;
    int i = 0;
    int status = 0;
    int ret = 0;
    
    if (!cmds)
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
            return (1);
        }
        if (pid == 0)
        {
            if (prev != -1)
            {
                dup2(prev, STDIN_FILENO);
                close(prev);
            }
            if (pipefd[1] != 0)
                dup2(pipefd[1], STDOUT_FILENO);
            if (pipefd[0] != -1)
                close(pipefd[0]);
            if (pipefd[1] != -1)
                close(pipefd[1]);
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
    pid = wait(&status);
    while (pid > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status))
            ret = 1;
        if (WIFSIGNALED(status))
            ret = 1;
        pid = wait(&status);
    }
    return (ret);
}
