#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define PORT 6000
#define BACKLOG 10
#define MAX_CHILDREN 10

#define MAX_LINE 20

void manage_connection(int connfd);

void sigchld_handler(int signum) {
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int main(int argc, char *argv[]) {
    signal(SIGCHLD, &sigchld_handler);

    int sockfd;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(PORT);

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("il padre entra nel loop di accettazione\n");
    
    for ( ; ; ) {
        int connfd;

        if ((connfd = accept(sockfd, NULL, 0)) == -1) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            close(connfd);
            continue;
        } else if (pid == 0) { /* child */
            close(sockfd);
            manage_connection(connfd);
            exit(EXIT_SUCCESS);
        }

        /* parent */
        printf("il padre chiude la connessione del client\n");


        close(connfd);
    }

    return EXIT_SUCCESS;
}

int my_readline(int connfd, char *buffer, size_t len);
ssize_t writen(int fd, const void * buffer, size_t n);

void manage_connection(int connfd) {
    printf("il figlio gestisce la connesione del client\n");

    char buffer[MAX_LINE + 1];

    while (1) {
        size_t nread = my_readline(connfd, (char *)&buffer, sizeof(buffer));

        if (nread == -1) {
            perror("my_readline");
            close(connfd);
            exit(EXIT_FAILURE);
        }

        printf("letta riga: %ld\n", nread);

        if (nread == 0) break;

        int len;
        if (buffer[nread - 1] == '\n') {
            len = nread - 1;
        } else {
            len = nread;
        }

        for (int i = 0 ; i < len/2 ; i++) {
            char t = buffer[i];
            buffer[i] = buffer[len - i - 1];
            buffer[len - i - 1] = t;
        }

        size_t nwritter = writen(connfd, buffer, nread); /* attenzione: e' una funzione ausiliaria */
        if (nwritter == -1) {
            perror("nwrite");
            exit(EXIT_FAILURE);
        }
    }

    printf("il figlio ha finito\n");

    close(connfd);
}

int my_readline(int connfd, char *buffer, size_t len) {
    static char internal_buffer[MAX_LINE + 1];
    static size_t alreadyRead = 0;

    if (len > sizeof(internal_buffer)) {
        errno = EINVAL;
        return -1;
    }

    size_t toRead = len - 1 - alreadyRead;
    char *nl = 0;
    while ((nl = (char *)memchr(internal_buffer, '\n', alreadyRead)) == NULL && toRead > 0) {
        printf("Going to read %ld byte\n", toRead);
        size_t nread = read(connfd, internal_buffer + alreadyRead, toRead);
        printf("Letti %ld byte\n", nread);
        
        if (nread == -1) {
            if (errno != EINTR) {
                return -1;
            }
        } else if (nread == 0) { /* EOF */
            break;
        } else {
            alreadyRead += nread;
            toRead -= nread;
        }
    }

    /* nl != NULL or toRead == 0 */
    printf("nl = %p alreadyRead = %ld", nl, alreadyRead);

    if (nl == NULL) {
        memcpy(buffer, internal_buffer, alreadyRead);
        buffer[alreadyRead] = '\0';
        int oldAlreadyRead = alreadyRead;
        alreadyRead = 0;
        return oldAlreadyRead;
    } else {
        size_t howmany = nl - internal_buffer + 1;
        memcpy(buffer, internal_buffer, howmany);
        buffer[howmany] = '\0';

        alreadyRead -= howmany;
        if (alreadyRead > 0) {
            memcpy(internal_buffer, internal_buffer + howmany, alreadyRead);
        }
        return howmany;
    }
}


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
