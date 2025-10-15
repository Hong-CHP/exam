/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 21:28:23 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 21:41:53 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int    ft_popen(const char *file, char *const argv[], int type)
{
    pid_t pid;
    int pipefd[2];
    
    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    pid = fork();
    if (pipe(pipefd) == -1)
        return (-1);
    if (pid < 0)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return (-1);
    }
    if (pid == 0)
    {
        if (type == 'r')
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }
        else
        {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }
        execvp(file, argv);
        exit(1);
    }
    else
    {
        if (type == 'r')
        {
            close(pipefd[1]);
            return (pipefd[0]);
        }
        else
        {
            close(pipefd[0]);
            return (pipefd[1]);         
        }
    }
}
