#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int countLines(char * fileName){
    FILE * file = fopen(fileName, "r");

    char line [512];
    int lineNumber = 0;

    while(fgets(line,512,file)!=NULL) {
        lineNumber++;
    }
    fclose(file);
    return lineNumber;
    
}

void getQuote(int number, char * fileName, char *buffer, int bufferSize){
    
    int counter = 1;
    FILE * file = fopen(fileName, "r");
    while(fgets(buffer,bufferSize,file)!=NULL) {        // strlen

        if(counter == number){
           break;
        }else{     
               counter++;

        }
    }
    fclose(file);
}

int main(int argc, char * argv[]){
    
    char * fileName = argv[2];
    //FILE * file = fopen(fileName, "r");

    /*if(file == NULL){
        printf("Datei ist leer.\n");
        return -1;
    }*/

    int lines = countLines(fileName);           // 32 zeilen
    int port = atoi(argv[1]);

    struct sockaddr_in serverSock;
    struct sockaddr_storage connector;
    socklen_t socketSize;
    memset(&serverSock,0,sizeof(serverSock));       // clean 
    
    serverSock.sin_family = AF_INET;    // wir sagen IPv4
    serverSock.sin_port = htons(port);  // wir geben Port
    serverSock.sin_addr.s_addr = inet_addr("127.0.0.1");        // gebe Adresse

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);         // erstelle Socket

    if(socketfd == -1){     // überprüfe Socket
        perror("");
        return -1;
    }

    int bindResult = bind(socketfd,(struct sockaddr *)&serverSock, sizeof(serverSock));       // rufe bind() auf um Socket adresse und port auf PC zuzuweisen

    if(bindResult < 0){
        perror("");
        return -1;  // exit(EXIT_FAILURE); 
    }

    int listenResult = listen(socketfd, 10); // zweite Zahl ist backlog ... kann irgendeine Zahl sein -> queue

    if (listenResult < 0){
        perror("");
        exit(EXIT_FAILURE);
    }

    int newSocketfd;
    char *buffer = (char *) malloc(512);       // vllt mit malloc, damit änderung auch nach Aufruf der funktion bestehen bleibt --> getQotes
    int randomNUmber;
    int unsigned bufferSize= 512;
    printf("Server is running...\n");
    while(1){
        time_t t;
        srand((unsigned) time(&t));
        randomNUmber = rand() % lines;
        getQuote(randomNUmber, fileName, buffer, bufferSize);         // --> Funktion nicht gut implementiert
        
        socketSize = sizeof(connector);
        newSocketfd = accept(socketfd, (struct sockaddr*)&connector, &socketSize);   // der neue Socketfd wird für das senden verwendet

        if(newSocketfd == -1){
            perror("");
        }

        send(newSocketfd, buffer, strlen(buffer), 0);
        close(newSocketfd);
    }
    
    close(socketfd);

    free(buffer);

    return 0;


}