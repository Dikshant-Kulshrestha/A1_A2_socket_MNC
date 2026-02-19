#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096


static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <server-ip> <server-port>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    long port_long = strtol(argv[2], &end, 10);
    if (!end || *end != '\0' || port_long <= 0 || port_long > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[2]);
        return 1;
    }

    int port = (int)port_long;

    // TODO: Create a TCP socket (AF_INET, SOCK_STREAM).

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }


       struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return 1;
    }

    // TODO: Populate sockaddr_in with server IP/port.
    // TODO: Connect to the server.


    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // TODO: Read from stdin in a loop (read()) and send in chunks.

    // TODO: For each chunk, send the *exact bytes* you read.
    //   - Use send()/write() in a loop to handle partial sends.
    //   - Do NOT add newlines, prefixes, or other formatting.
    // The test harness compares server stdout byte-for-byte with stdin input.

    char buf[BUF_SIZE];
    ssize_t nread;

    while ((nread = read(STDIN_FILENO, buf, sizeof(buf))) != 0) {
        if (nread < 0) {
            if (errno == EINTR) continue; // retry
            perror("read");
            break;
        }

        ssize_t total_sent = 0;
        while (total_sent < nread) {
            ssize_t nsent = send(sockfd, buf + total_sent, nread - total_sent, 0);
            if (nsent < 0) {
                if (errno == EINTR) continue; // retry
                perror("send");
                close(sockfd);
                return 1;
            }
            total_sent += nsent;
        }
    }


    // TODO: Close the socket before exiting.

    close(sockfd);

    return 0;
}
