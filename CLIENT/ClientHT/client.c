#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#define WAIT_FOR 2000000
#define HEADER_LENGTH 6
#define SET_Com "SET"
#define GET_Com "GET"
#define DELETE_Com "DELETE"
#define SET 1
#define GET 2
#define DELETE 3
#define BUFFER_SIZE 1024
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


struct Message {
    uint8_t header[HEADER_LENGTH];
    char keyBuffer[BUFFER_SIZE];
    char valBuffer[BUFFER_SIZE];
    int keyLength;
    int valLength;
    char command[BUFFER_SIZE];

};


void clear_and_init_message_container(struct Message *message) {
    memset(message->header, 0, HEADER_LENGTH);
    memset(message->keyBuffer, 0, BUFFER_SIZE);
    memset(message->valBuffer, 0, BUFFER_SIZE);
    message->keyLength = 0;
    message->valLength = 0;
}

int has_data_available(int socketfd, uint32_t waitTimeUs) {
    fd_set fds;
    int16_t monitorDeskr = 0;
    struct timeval timeout;
    struct timeval *timeoutPtr = NULL;

    if (waitTimeUs > 0) {
        timeout.tv_sec = waitTimeUs / 1000000;
        timeout.tv_usec = waitTimeUs % 1000000;
        timeoutPtr = &timeout;
    }
    FD_SET(socketfd, &fds);
    monitorDeskr = (int16_t) select(socketfd + 1, &fds, NULL, NULL, timeoutPtr);
    if (monitorDeskr == -1) {
        perror("select failed");
        return 1;
    }
    if (!FD_ISSET(socketfd, &fds)) {
        return 0;
    } else {
        return 1;
    }
}

int set_bit_at(uint8_t byte, int pos) {
    byte |= 1UL << pos;
    return byte;
}


int search_for_command(struct Message *message) {

    if (strcmp(message->command, DELETE_Com) == 0) {
        message->header[0] = (uint8_t) set_bit_at(message->header[0], 0);
        return 3;
    } else if (strcmp(message->command, GET_Com) == 0) {
        message->header[0] = (uint8_t) set_bit_at(message->header[0], 2);
        return 2;
    } else if (strcmp(message->command, SET_Com) == 0) {
        message->header[0] = (uint8_t) set_bit_at(message->header[0], 1);
        return 1;
    } else {
        return 0;
    }
}


int write_exactly_n_bytes(int socketfd, uint8_t *bufferPointer, ssize_t expectedSize) {
    size_t totalInput = (size_t) expectedSize;
    while (totalInput > 0) {
        ssize_t current = write(socketfd, bufferPointer, totalInput);
        if (current < 0) {
            perror("Write");
            return 1;
        } else {
            totalInput = totalInput - current;
            bufferPointer = bufferPointer + current;
        }
    }
    return 0;
}

int send_message(int socketfd, struct Message *message) {
    message->header[2] = (uint8_t) ((message->keyLength >> 8) & 0xff);
    message->header[3] = (uint8_t) (message->keyLength & 0xff);
    message->header[4] = (uint8_t) ((message->valLength >> 8) & 0xff);
    message->header[5] = (uint8_t) (message->valLength & 0xff);
    if (write_exactly_n_bytes(socketfd, message->header, HEADER_LENGTH) != 0) {
        perror("Write header");
        return 1;
    }
    if (message->keyLength > 0) {
        if (write_exactly_n_bytes(socketfd, (uint8_t *) message->keyBuffer, message->keyLength) != 0) {
            perror("Write key data");
            return 1;
        }
    }
    if (message->valLength > 0) {
        if (write_exactly_n_bytes(socketfd, (uint8_t *) message->valBuffer, message->valLength) != 0) {
            perror("Write value data");
            return 1;
        }
    }

    return 0;
}

int read_exactly_n_bytes(int socketfd, uint8_t *bufferPointer, ssize_t expectedSize) {
    size_t totalInput = (size_t) expectedSize;
    while (totalInput > 0) {
        ssize_t current = read(socketfd, bufferPointer, totalInput);
        if (current < 0) {
            perror("Read");
            return 1;
        } else {
            totalInput = totalInput - current;
            bufferPointer = bufferPointer + current;
        }
    }
    return 0;
}

void char_to_short(uint8_t first, uint8_t second, uint16_t *pshort) {
    *pshort = ((uint16_t) first << 8) | (uint16_t) second;
}

int read_message(int socketfd, struct Message *message) {
    if (read_exactly_n_bytes(socketfd, message->header, HEADER_LENGTH) != 0) {
        perror("Read header");
        return 1;
    }
    char_to_short(message->header[2], message->header[3], (uint16_t *) &message->keyLength);
    char_to_short(message->header[4], message->header[5], (uint16_t *) &message->valLength);

    if (message->keyLength > 0) {
        if (read_exactly_n_bytes(socketfd, (uint8_t *) message->keyBuffer, message->keyLength) != 0) {
            perror("Read key data");
            return 1;
        }
    }

    if (message->valLength > 0) {
        if (read_exactly_n_bytes(socketfd, (uint8_t *) message->valBuffer, message->valLength) != 0) {
            perror("Read value data");
            return 1;
        }
    }
    return 0;
}

int request(int socketfd, struct Message *message) {
    if (send_message(socketfd, message) != 0) {
        perror("While sending");
        return 1;
    }
    return 0;
}

int response(int socketfd, struct Message *message) {
    if (read_message(socketfd, message) != 0) {
        perror("While reading");
        return 1;
    }
    return 0;
}

void print_message(struct Message *message) {
    //  printf("Bits: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(message->header[0]));
    //  printf("key: %s\n", message->keyBuffer);
    //  printf("keyLen: %d\n", message->keyLength);
    printf("%s\n", message->valBuffer);
    //  printf("valueLen: %d\n", message->valLength);
    //  printf("transaction key: %d\n", message->header[1]);
}

void send_request(int socketfd, struct Message *message) {
    // printf("=== request ===\n");
    if (request(socketfd, message) != 0) {
        perror("Create packet");
    }
    // print_message(message);
}

int is_bit_set_at(uint8_t byte, int pos) {
    return (byte >> pos) & 0x01;
}

void manage_command(int socketfd, struct Message *message) {
    send_request(socketfd, message);

    while(has_data_available(socketfd, WAIT_FOR) == 0) {
        //   printf("# Server did not respond, re-attempt sending request\n");
        send_request(socketfd, message);
    }

    // printf("=== response ===\n");

    clear_and_init_message_container(message);

    if (response(socketfd, message) != 0) {
        perror("Response");
    }
}


int main(int argc, char *argv[]) {
    struct Message message;
    clear_and_init_message_container(&message);

    if (argc < 5 || argc > 7) {
        perror("Wrong input\n");
        return 1;
    }
    char *host = argv[1];
    char *port = argv[2];
    strcpy(message.command, argv[3]);
    strcpy(message.keyBuffer, argv[4]);
    message.keyLength = (int) strlen(message.keyBuffer);

    char *transId = malloc(1 * sizeof(char));
    int socketfd = 0;
    int connectfd = 0;

    struct addrinfo clientConfiguration;
    struct addrinfo *connectionInfo, *rp;
    memset(&clientConfiguration, 0, sizeof clientConfiguration);
    clientConfiguration.ai_family = AF_INET;
    clientConfiguration.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &clientConfiguration, &connectionInfo) != 0) {
        perror("Getaddrinfo");
        return 1;
    }
    for (rp = connectionInfo; rp != NULL; rp = rp->ai_next) {
        socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socketfd == -1) {
            continue;
        }
        connectfd = connect(socketfd, rp->ai_addr, rp->ai_addrlen);
        if (connectfd < 0) {
            perror("Connect");
            return 1;
        }
    }
    if (argc == 5) {
        strcpy(message.valBuffer, "");
        message.valLength = 0;
    } else if (argc == 6) {
        strcpy(message.valBuffer, argv[5]);
        message.valLength = (int) strlen(message.valBuffer);
    } else {
        strcpy(message.valBuffer, argv[5]);
        message.valLength = (int) strlen(message.valBuffer);
        message.header[1] = (uint8_t) strtol(argv[6], NULL, 0);
    }

    int command_is = search_for_command(&message);

    if (command_is == SET) {
        manage_command(socketfd, &message);
        //print_message(&message);
    } else if (command_is == GET) {
        manage_command(socketfd, &message);
        if(is_bit_set_at(message.header[0],2) == 1){
            if(is_bit_set_at(message.header[0],3) == 1){
                printf("%s\n", message.valBuffer);
            }
        }
        //print_message(&message);
    } else if (command_is == DELETE) {
        manage_command(socketfd, &message);
        // print_message(&message);
    } else {
        perror("Command does not exist");
        return 1;
    }
    close(connectfd);
    close(socketfd);
    freeaddrinfo(connectionInfo);

    return 0;
}
