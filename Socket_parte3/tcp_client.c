#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PORT 6000

#define MAX_LINE 20

/*
    Funzione ausiliaria per trasmettere tutti e n i byte
*/
ssize_t writen(int fd, const void * buffer, size_t n) {
    size_t toSend = n;
    const void * sendBuffer = buffer;
    while (toSend > 0) {
        ssize_t nwritten = write(fd, sendBuffer, toSend);
        if (nwritten == -1) { /* restituisce il numero di byte scritti oppure -1 in caso di errore*/
            if (errno == EINTR) continue; /* non e' un errore, continua con la scrittura */
            
            return -1;
        }

        toSend -= nwritten;
        sendBuffer = (void*)((char *)sendBuffer + nwritten);
    }

    return n;
}

int main(int argc, char *argv[]) {

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);        
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];

    if (fgets((char *)&buffer, sizeof(buffer), stdin)) {
        ssize_t nwritten = writen(sockfd, buffer, strlen(buffer)); /* funzione ausiliaria */

        if (nwritten == -1) {
            perror("writen");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if (shutdown(sockfd, SHUT_WR) == -1) {
            perror("shutdown");
            close(sockfd);
            exit(EXIT_FAILURE);
        }


        while (1) {
            ssize_t nread = read(sockfd, &buffer, sizeof(buffer));
            if (nread == -1) {
                perror("read");
                close(sockfd);
                exit(EXIT_FAILURE);
            }

            if (nread == 0) break;

            fwrite(buffer, 1, nread, stdout);
        }


        close(sockfd);

        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;
}