#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>

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

unint32_t ip_to_hex() {
    int nums = [127, 0, 0, 1];
    unint32_t res = nums[0] << 24 | nums[1] << 16 | nums[2] << 8 | nums[3];
    return res;
}

void fatal_error() {
    write(2, "Fatal error\n", 12);
    exit 1;
}



int main(int argc, char **argv) {
    if (argc != 2) {
        write(2, "Wrong number of arguments\n", 26);
        exit 1;
    }
    if (valid_port(argv[1]))
        fatal_error();
    int port = atoi(argv[1]);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0); 
    if (listenFd == -1)
        fatal_error();
    const struct sockaddr_in serv_addr;
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

    fd_set rset, rtemp;
    int max_fd = listenFd;

    FD_ZERO(&rset);
    FD_SET(listenFd, &rset);
    while (1) {
        rtemp = rset;
        int num = select(max_fd + 1, &rtemp, NULL, NULL, NULL);
        if (num == -1) {
            close(listenFd);
            fatal_error();
        }
        if (FD_ISSET(listenFd, &rtemp)) {
            struct sockaddr_in cli_addr;
            int cli_len = sizeof(cli_addr);
            int cfd = accept(listenFd, (struct sockaddr*)&cli_addr, &cli_len);
            FD_SET(cfd, &rset);
            
        }
    }
    return 0;
}