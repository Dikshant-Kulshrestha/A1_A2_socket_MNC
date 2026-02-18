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
    fprintf(stderr, "Usage: %s <listen-port>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    long port_long = strtol(argv[1], &end, 10);
    if (!end || *end != '\0' || port_long <= 0 || port_long > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[1]);
        return 1;
    }

    int port = (int)port_long;


    // TODO: Create a TCP listen socket (AF_INET, SOCK_STREAM).

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return 1;
    }
    // TODO: Set SO_REUSEADDR on the listen socket.

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(listen_fd);
        return 1;
    }

    // TODO: Bind the socket to INADDR_ANY and the given port.

     struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return 1;
    }

    // TODO: Listen with a small backlog (e.g., 5-10).

    if (listen(listen_fd, 5) < 0) {
        perror("listen");
        close(listen_fd);
        return 1;
    }

    // TODO: Accept clients in an infinite loop.
    //   - For each client, read in chunks until EOF.
    //   - For each chunk, write those *exact bytes* to stdout.
    //     Use write(STDOUT_FILENO, ...) in a loop to handle partial writes.
    //   - Do NOT use printf/fputs or add separators/newlines/prefixes.
    //   - The test harness compares server stdout byte-for-byte with client input.

        while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_fd;

        while ((client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen)) < 0) {
            if (errno == EINTR) continue; // interrupted, retry
            perror("accept");
            close(listen_fd);
            return 1;
        }

        // --- Read from client and write to stdout ---

            char buf[BUF_SIZE];
        ssize_t nread;
        while ((nread = read(client_fd, buf, sizeof(buf))) != 0) {
            if (nread < 0) {
                if (errno == EINTR) continue; // retry
                perror("read");
                break;
            }

            ssize_t total_written = 0;
            while (total_written < nread) {
                ssize_t nwritten = write(STDOUT_FILENO, buf + total_written, nread - total_written);
                if (nwritten < 0) {
                    if (errno == EINTR) continue; // retry
                    perror("write");
                    break;
                }
                total_written += nwritten;
            }
        }

        close(client_fd); // close this client
    }
    // TODO: Handle EINTR and other error cases as specified.

    // TODO: Close the listen socket before exiting.

    close(listen_fd);

    return 0;
}
