#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define MAX_BUFFER_SIZE 512
#define BILLION  1000000000L;

int main(int argc, char *argv[]) {
    if(argc != 3){
        perror("Wrong argc number");
        return -1;
    }
    struct timespec requestStart, requestEnd;
    char *dns = argv[1];
    char *port = argv[2];
    char buffer[MAX_BUFFER_SIZE] = "";
    int socketfd = 0;
    socklen_t fromlen;
    struct sockaddr_storage addr;

    struct addrinfo clientConfiguration;
    struct addrinfo *connectionInfo;


    memset(&clientConfiguration, 0, sizeof(clientConfiguration));
    clientConfiguration.ai_family = AF_INET;
    clientConfiguration.ai_socktype = SOCK_DGRAM;

    clock_gettime(CLOCK_REALTIME, &requestStart);

    int result = getaddrinfo(dns, port, &clientConfiguration, &connectionInfo);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo error");
        return 1;
    }

    //Go through the LInked list !!  for loop
    socketfd = socket(connectionInfo->ai_family, connectionInfo->ai_socktype, connectionInfo->ai_protocol);

    if (socketfd == -1) {
        perror("Socket ");
        return -1;
    }

    if (sendto(socketfd, "HI\0", 2, 0, connectionInfo->ai_addr, connectionInfo->ai_addrlen) == -1) {
        perror("sendto");
        return 1;
    }

    fromlen = sizeof(addr);

    int numberOfButes = (int) recvfrom(socketfd, &buffer, MAX_BUFFER_SIZE - 1, 0, (struct sockaddr *) &addr, &fromlen);

    clock_gettime(CLOCK_REALTIME, &requestEnd);
    buffer[numberOfButes] = '\0';
    printf("%s\n", buffer);


    freeaddrinfo(connectionInfo);
    close(socketfd);

    double translation = ( requestEnd.tv_sec - requestStart.tv_sec ) + (double)( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
    printf( "accum: %lf\n", translation );

    return 0;
}

