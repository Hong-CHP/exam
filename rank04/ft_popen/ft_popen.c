/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 17:36:00 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 18:29:51 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int    ft_popen(const char *file, char *const argv[], int type)
{
	pid_t pid;
	int pipefd[2];
	
	if (!file || !argv || !argv[1] || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
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
		else if (type == 'w')
		{
			close(pipefd[0]);
			return (pipefd[1]);
		}
	}
}

int main(int argc, char *argv[])
{
	int fd5 = ft_popen("true", (char *const[]){"true", NULL}, 'r');
	char buf1[16];
	int n6 = read(fd5, buf1, sizeof(buf1)); // 应该没有输出
	close(fd5);
	wait(NULL);
	printf("read returned %d\n", n6);   // 期望 0

	int fd6 = ft_popen("yes", (char *const[]){"yes", NULL}, 'r');
	char buf2[1024];
	int n1 = read(fd6, buf2, sizeof(buf2)-1);
	buf2[n1] = '\0';
	close(fd6);
		wait(NULL);
	printf("first chunk: %.50s\n", buf2);

	for (int i = 0; i < 5; i++) {
	    int fd7 = ft_popen("echo", (char *const[]){"echo", "hi", NULL}, 'r');
	    char buf3[16];
	    int n2 = read(fd7, buf3, sizeof(buf3)-1);
	    buf3[n2] = '\0';
	    printf("%d: %s", i, buf3);
	    close(fd7);               // ✅ 关闭 fd
	    wait(NULL);               // ✅ 回收子进程
	}
}