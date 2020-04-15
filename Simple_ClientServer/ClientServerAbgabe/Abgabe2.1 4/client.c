#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char * argv[]){

    char * dns = argv[1];
    char * port = argv[2];
    char buffer[1024] = "";

    struct addrinfo clientConfiguration;
    struct addrinfo * connectionInfo;
    memset(&clientConfiguration, 0, sizeof clientConfiguration);
    clientConfiguration.ai_family = AF_INET;  //IP4
    clientConfiguration.ai_socktype = SOCK_STREAM;    // stream

    int x = getaddrinfo(dns, port, &clientConfiguration , &connectionInfo);
    
    if(x != 0){
       // gai_sterror(x);
        return -1;
    }

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

    printf("\n");

    close(socketfd);

    freeaddrinfo(connectionInfo);

    return 0;


}