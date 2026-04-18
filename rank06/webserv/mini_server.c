#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

fd_set readfd, writefd, allfd;
int     id[65536];
char    *msgs[65536];
char    buf_write[1024];
char    buf_read[1024];
int     count = 0;
int     maxfd= 0;

void fatal_error(int num);
uint32_t    ipToHex();
int socket_init_bind_listen(int port);
void    notify_others(int c_fd, char *buf);
void    remove_client(int fd);

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

int main(int argc, char **argv) {
    if (argc != 2) {
        write(2, "Wrong number of arguments\n", 26);
        exit(1);
    }
    int port = atoi(argv[1]);
    int sock_fd = socket_init_bind_listen(port);
    maxfd = sock_fd;
    FD_ZERO(&allfd);
    FD_SET(sock_fd, &allfd);
    while (1) {
        readfd = allfd;
        writefd = allfd;
        int numfd = select(maxfd + 1, &readfd, &writefd, NULL, NULL);
        if (numfd == -1)
            fatal_error(sock_fd);
        for (int i = 0; i < maxfd + 1; ++i) {
            if (!FD_ISSET(i, &readfd))
                continue ;
            if (i == sock_fd) {
                struct sockaddr_in cli_addr;
                socklen_t cli_addr_len = sizeof(cli_addr);
                int c_fd = accept(i, (struct sockaddr *)&cli_addr, &cli_addr_len);
                if (c_fd == -1)
                    continue ;
                FD_SET(c_fd, &allfd);
                maxfd = c_fd > maxfd ? c_fd : maxfd;
                id[c_fd] = count++;
                msgs[c_fd] = NULL;
                sprintf(buf_write, "server: client %d just arrived\n", id[c_fd]);
                notify_others(c_fd, buf_write);
            } else {
                int recv_bytes = recv(i, buf_read, sizeof(buf_read) - 1, 0);
                if (recv_bytes <= 0) {
                    if (recv_bytes == 0) {
                        sprintf(buf_write, "server: client %d just left\n", id[i]);
                        notify_others(i, buf_write);
                    }
                    free(msgs[i]);
                    msgs[i] = NULL;
                    remove_client(i);
                } else {
                    buf_read[recv_bytes] = '\0';
                    msgs[i] = str_join(msgs[i], buf_read);
                    if (msgs[i] == NULL)
                        fatal_error(sock_fd);
                    char *msg;
                    while (extract_message(&msgs[i], &msg)) {
                        sprintf(buf_write, "client %d: ", id[i]);
                        notify_others(i, buf_write);
                        notify_others(i, msg);
                        free(msg);
                    }
                }
            }
        }
    }
    return 0;
}

void    remove_client(int fd) {
    FD_CLR(fd, &allfd);
    close(fd);
}

void    notify_others(int c_fd, char *buf) {
    for (int i = 0; i < maxfd + 1; ++i) {
        if (FD_ISSET(i, &writefd) && i != c_fd)
            send(i, buf, strlen(buf), 0);
    }
}

int socket_init_bind_listen(int port) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
        fatal_error(-1);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(ipToHex());
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        fatal_error(sock_fd);
    if (listen(sock_fd, 128) < 0)
        fatal_error(sock_fd);

    return sock_fd;
}

uint32_t    ipToHex() {
    int nums[4] = {127, 0, 0, 1};
    uint32_t ip = nums[0] << 24 | nums[1] << 16 | nums[2] << 8 | nums[3];
    return ip;
}

void fatal_error(int num) {
    if (num != -1)
        close(num);
    write(2, "Fatal error\n", 12);
    exit(1);
}