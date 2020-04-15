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
#define BILLION  1000000000L;

int main(int argc, char * argv[]){
    if(argc != 3){
        perror("Wrong argc number");
        return -1;
    }

    char * dns = argv[1];
    char * port = argv[2];
    char buffer[1024] = "";
    struct timespec requestStart, requestEnd;

    struct addrinfo clientConfiguration;
    struct addrinfo * connectionInfo;
    memset(&clientConfiguration, 0, sizeof clientConfiguration);
    clientConfiguration.ai_family = AF_INET;
    clientConfiguration.ai_socktype = SOCK_STREAM;

    int result = getaddrinfo(dns, port, &clientConfiguration , &connectionInfo);

    if(result != 0){
        return -1;
    }
    clock_gettime(CLOCK_REALTIME, &requestStart);
    int socketfd = socket(connectionInfo-> ai_family, connectionInfo -> ai_socktype, connectionInfo -> ai_protocol);

    if(socketfd == -1){
        perror("Problem with Socket: ");
        return -1;
    }

    int c = connect(socketfd, connectionInfo -> ai_addr, connectionInfo ->ai_addrlen);

    if (c < 0)
    {
        perror("Problem with connect: ");
    }

    while(recv(socketfd, &buffer, 1023, 0) > 0){
        printf("%s", buffer);

    }
    clock_gettime(CLOCK_REALTIME, &requestEnd);

    close(socketfd);

    freeaddrinfo(connectionInfo);

    double translation = ( requestEnd.tv_sec - requestStart.tv_sec ) +(double) ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
    printf( "%lf\n", translation );

    return 0;


}