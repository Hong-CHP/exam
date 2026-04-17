#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>

fd_set read_fds, write_fds, all_fds;
int count = 0;
int max_fd = 0;
int client_id[65535];
char    *msgs[65535];
char    buf_write[1024];
char    buf_read[1024];

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

bool valid_port(char *str) {
    int len = 0;
    while (str[len]) {
        if (!(str[len] >= '0' && str[len] <= '9'))
            return false;
        len++;
    }
    if (len != 4)
        return false;
    return true;
}

uint32_t ip_to_hex() {
    int nums[4] = {127, 0, 0, 1};
    uint32_t res = nums[0] << 24 | nums[1] << 16 | nums[2] << 8 | nums[3];
    return res;
}

void fatal_error() {
    write(2, "Fatal error\n", 12);
    exit (1);
}

void    notify(int clientFd, char *buf_write) {
    for (int i = 0; i < max_fd + 1; ++i) {
        if (FD_ISSET(i, &wfds) && i != client_fd)
            send(i, buf_write, strlen(buf_write), 0);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        write(2, "Wrong number of arguments\n", 26);
        exit (1);
    }
    if (!valid_port(argv[1]))
        fatal_error();
    int port = atoi(argv[1]);

    int listenFd = socket(AF_INET, SOCK_STREAM, 0); 
    if (listenFd == -1)
        fatal_error();
    
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(ip_to_hex());
    if (bind(listenFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        close(listenFd);
        fatal_error();
    }
    if (listen(listenFd, 128) == -1) {
        close(listenFd);
        fatal_error();
    }

    FD_ZERO(&all_fds);
    FD_SET(listenFd, &all_fds);
    max_fd = listenFd;
    while (1) {
        read_fds = all_fds;
        write_fds = all_fds;

        int nums = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);
        if (nums < 0) {
            close(listenFd);
            fatal_error();
        }

        for (int i = 0; i < max_fd + 1; ++i) {
            if (!FD_ISSET(i, &read_fds))
                continue ;
            if (i == listenFd)
            {
                struct sockaddr_in client_addr;
                int client_addr_len = sizeof(client_addr);
                int client_fd = accept(listenFd, (struct sockaddr*)client_addr, &client_addr_len);
                if (client_fd < 0)
                    continue ;
                FD_SET(client_fd, &all_fds);
                max_fd = client_fd > max_fd ? client_fd : max_fd;
                client_id[client_fd] = count++;
                msgs[client_fd] = NULL;
                sprintf(buf_write, "Client %d connect", client_id[client_fd]);
                notify(client_fd, buf_write);
            }
            else 
            {
                int recv_bytes = recv(i, buf_read, sizeof(buf_read), 0);
                if (recv_bytes < =0) {
                    if (recv_bytes == 0)
                        sprintf(buf_write, "Client %d left", client_id[i]);
                    notify(i, buf_write);
                    free(msgs[i]);
                    FD_CLR(i, &all_fds);
                    close(i);
                }
                else {
                    buf_read[recv_bytes] = '\0';
                    msgs[i] = str_join(msgs[i], buf_read);
                    char *msg;
                    while (extract_message(&msgs[i], &msg)) {
                        sprintf(buf_write, "Client %d:", client_id[i]);
                        notify(i, buf_write);
                        notify(i, msg);
                        free(msg);
                    }
                }
            }
        }
    }
    return 0;
}
