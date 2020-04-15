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
#include "uthash.h"

# define HEADER_LENGTH 6

/*
uint8_t 8 Bit   0..255  unsigned char
uint16_t 16 Bit 0..65535    unsigned int
*/

struct Message {
    uint8_t header[HEADER_LENGTH];
    uint8_t keyBuffer[UINT16_MAX];//signed 16-bit integer.
    uint8_t valBuffer[UINT16_MAX];
    uint16_t keyLength;
    uint16_t valLength; //2 byte
};//here could be an alias


struct HashMapEntry {
    uint8_t keyBuffer[UINT16_MAX];
    uint8_t valBuffer[UINT16_MAX];
    uint16_t valLength;
    UT_hash_handle hh; // hh. This allows you to use the easier "convenience" macros to add, find and delete items.
};

struct HashMapEntry *entries = NULL;


/*
translation Bytes to Bits ->     // https://stackoverflow.com/questions/25787349/convert-char-to-short

bitwise manipulation
>> = Rechts schieben
<< = Links schieben (Bsp: a<<b ist das gleiche wie a * 2^b; bzw. bei 1<<3 wird die 1 um drei Stellen nach links geschoben)
| = binäres ODER
& = binäres UND
^ = binäres Exklusives ODER
 */
void char_to_short(uint8_t first, uint8_t second, uint16_t *pshort) {
    *pshort = ((uint16_t) first << 8) | (uint16_t) second;
}

// https://stackoverflow.com/questions/8136776/blocking-read-until-specified-number-bytes-arrived
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

/*
 * clear struct before receive
 */
void clear_and_init_message_container(struct Message *message) {
    memset(message->header, 0, HEADER_LENGTH);
    memset(message->keyBuffer, 0, UINT16_MAX);
    memset(message->valBuffer, 0, UINT16_MAX);

    message->keyLength = 0;
    message->valLength = 0;
}
void clear_key_and_value_from_message(struct Message *message) {
    memset(message->keyBuffer, 0, UINT16_MAX);
    message->keyLength = 0;
    memset(message->valBuffer, 0, UINT16_MAX);
    message->valLength = 0;
}

/*
 * Read first messages from Client and cut them for translation
 */

int read_message(int socketfd, struct Message *message) {
    if (read_exactly_n_bytes(socketfd, message->header, HEADER_LENGTH) != 0) {
        perror("Read header");
        return 1;
    }

    char_to_short(message->header[2], message->header[3], &message->keyLength);
    char_to_short(message->header[4], message->header[5], &message->valLength);

    if (message->keyLength > 0) {
        if (read_exactly_n_bytes(socketfd, message->keyBuffer, message->keyLength) != 0) {
            perror("Read key data");
            return 1;
        }
    }

    if (message->valLength > 0) {
        if (read_exactly_n_bytes(socketfd, message->valBuffer, message->valLength) != 0) {
            perror("Read value data");
            return 1;
        }
    }
    return 0;
}

/*
 * put all messages together and send them to the client in the same format
 */
/*bitwise manipulation
>> = Rechts schieben
<< = Links schieben (Bsp: a<<b ist das gleiche wie a * 2^b; bzw. bei 1<<3 wird die 1 um drei Stellen nach links geschoben)
| = binäres ODER
& = binäres UND
^ = binäres Exklusives ODER
*/
int send_message(int socketfd, struct Message *message) {
    message->header[2] = (uint8_t) ((message->keyLength >> 8) & 0xff); // to network byte order
    message->header[3] = (uint8_t) (message->keyLength & 0xff);

    message->header[4] = (uint8_t) ((message->valLength >> 8) & 0xff);
    message->header[5] = (uint8_t) (message->valLength & 0xff);
    if (write_exactly_n_bytes(socketfd, message->header, HEADER_LENGTH) != 0) {
        perror("Write header");
        return 1;
    }

    if (message->keyLength > 0) {
        if (write_exactly_n_bytes(socketfd, message->keyBuffer, message->keyLength) != 0) {
            perror("Write key data");
            return 1;
        }
    }

    if (message->valLength > 0) {
        if (write_exactly_n_bytes(socketfd, message->valBuffer, message->valLength) != 0) {
            perror("Write value data");
            return 1;
        }
    }

    return 0;
}
/*
 * Method tests if a client still has data,
 * without this function,read function will blocked and we can't read more data
 *
 * Problem because of separate reading
 */
int has_data_available(int socketfd, uint32_t waitTimeUs) {
    fd_set fds;
    int16_t monitorDeskr = 0;
    struct timeval timeout;
    struct timeval *timeoutPtr = NULL;

    //until the specified condition is true for at least one of the specified file descriptors or until a signal arrives that needs to be delivered.
    if (waitTimeUs > 0) {
        timeout.tv_sec = waitTimeUs / 1000000; //sec
        timeout.tv_usec = waitTimeUs % 1000000; //microsec
        timeoutPtr = &timeout;
    }
    //Initializes all on zero
    FD_ZERO(&fds);
    //set sockt to that bit
    FD_SET(socketfd, &fds);

    //select() -> monitor file descriptor and wait until it becomes ready to read
    //socketfd + 1 -> argument specifies the range of file descriptors to be tested
    //fd_set *readfds to test, write and except are NULL
    monitorDeskr = (int16_t) select(socketfd + 1, &fds, NULL, NULL, timeoutPtr);
    if (monitorDeskr == -1) {
        perror("select failed");
        return -1;
    }
    //non-zero value if the bit for the file descriptor fd is set pointed to by fdset, 0 otherwise.
    if (!FD_ISSET(socketfd, &fds)) {
        return 0; // no data
    } else {
        return 1;//has data
    }
}
/*
 * https://stackoverflow.com/questions/12791864/c-program-to-check-little-vs-big-endian
 * https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
 */

/*
bitwise manipulation
>> = Rechts schieben
<< = Links schieben (Bsp: a<<b ist das gleiche wie a * 2^b; bzw. bei 1<<3 wird die 1 um drei Stellen nach links geschoben)
| = binäres ODER
& = binäres UND
^ = binäres Exklusives ODER
 */
int is_bit_set_at(uint8_t byte, int pos) {
    return (byte >> pos) & 0x01; // hexadec.
}

int clear_bit_at(uint8_t byte, int pos) {
    byte &= ~(1UL << pos);
    return byte;
}
/*
 * Search for a command
 */
int is_del_command(struct Message *message) {
    return is_bit_set_at(message->header[0], 0);
}

int is_set_command(struct Message *message) {
    return is_bit_set_at(message->header[0], 1);
}

int is_get_command(struct Message *message) {
    return is_bit_set_at(message->header[0], 2);
}

int is_ack_command(struct Message *message) {
    return is_bit_set_at(message->header[0], 3);
}
// ^= 1UL << n ---> used to toggle a bit.
void set_acknowledge_bit_for(struct Message *message) {
    message->header[0] ^= 1UL << 3;
}
/*
 * Clean command after success activity
 */
void clear_command_bits_for(struct Message *message) {
    int i = 0;
    while (i <= 3) {
        message->header[0] = (uint8_t) clear_bit_at(message->header[0], i);
        i++;
    }
}

struct HashMapEntry *create_hashmap_entry_from(struct Message *message) {
    struct HashMapEntry *entry = (struct HashMapEntry *) malloc(sizeof(struct HashMapEntry));

    memcpy(entry->keyBuffer, message->keyBuffer, message->keyLength);
    memcpy(entry->valBuffer, message->valBuffer, message->valLength);

    entry->valLength = message->valLength;
    return entry;
}

/*
 * PERFORM used for safe/find/delete data to hash
 * clean command
 */
int perform_set(struct Message *message) {
    struct HashMapEntry *entry = NULL;

    HASH_FIND(hh, entries, message->keyBuffer, message->keyLength, entry);

    if (entry == NULL) {
        entry = create_hashmap_entry_from(message);
        HASH_ADD(hh, entries, keyBuffer, message->keyLength, entry);
    } else {
        // entry with key already exists, just replace value
        memcpy(entry->valBuffer, message->valBuffer, message->valLength);
        entry->valLength = message->valLength;
    }
    clear_key_and_value_from_message(message);
    return 1;
}

int perform_get(struct Message *message) {
    struct HashMapEntry *entry = NULL;
    HASH_FIND(hh, entries, message->keyBuffer, message->keyLength, entry);

    if (entry != NULL) {
        memcpy(message->valBuffer, entry->valBuffer, entry->valLength);
        message->valLength = entry->valLength;
        return 1;
    } else {
        return 0;
    }
}

int perform_del(struct Message *message) {
    struct HashMapEntry *entry = NULL;
    HASH_FIND(hh, entries, message->keyBuffer, message->keyLength, entry);

    clear_key_and_value_from_message(message);

    if (entry != NULL) {
        HASH_DEL(entries, entry);
        free(entry);
        return 1;
    } else {
        return 0;
    }
}
/*
 * Find nessessary operation
 */
void process_hashmap_operation(struct Message *message) {
    int is_success = 0;

    if (is_set_command(message)) {
        is_success = perform_set(message);
    } else if (is_get_command(message)) {
        is_success = perform_get(message);
    } else if (is_del_command(message)) {
        is_success = perform_del(message);
    }
    if (!is_success) {
        // clear header bits
        clear_command_bits_for(message);
    }
    // always only ACK, in case of no success, other header bits are cleared
    set_acknowledge_bit_for(message);
}

/*
 * Only for tests
 */
/*void print_message(struct Message *message) {

    printf("set: %d\n", is_set_command(message));
    printf("del: %d\n", is_del_command(message));
    printf("ack: %d\n", is_ack_command(message));
    printf("get: %d\n", is_get_command(message));
    printf("key: %s\n", message->keyBuffer);
    printf("keyLen: %d\n", message->keyLength);
    printf("value: %s\n", message->valBuffer);
    printf("valueLen: %d\n", message->valLength);
    printf("transaction key: %d\n", message->header[1]);
}
*/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Invalid number of argc");
        return 1;
    }

    int socketfd = 0;
    char *port = argv[1];
    char *host = "localhost";
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    int connectfd = 0;
    struct sockaddr_in client;
    socklen_t addrlen;
    //to avoid dynamic space allocation
    struct Message msg;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(host, port, &hints, &result) != 0) {
        perror("Getaddrinfo error");
        return 1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socketfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socketfd == -1) {
            continue;
        }
        if ((bind(socketfd, rp->ai_addr, rp->ai_addrlen)) == 0) {
            break;
        }
    }

    if (rp == NULL) {
        perror("Bind");
        return 1;
    }
    //support values of backlog up to SOMAXCONN
    if ((listen(socketfd, SOMAXCONN)) == -1) {
        perror("listen");
        return 1;
    }

    while (1) {
        addrlen = sizeof(client);
        connectfd = accept(socketfd, (struct sockaddr *) &client, &addrlen);
        if (connectfd == -1) {
            perror("Accept");
            return 1;
        }
        // printf("=== accept ===\n");
        printf("client connected from %s\n", inet_ntoa(client.sin_addr));

        while (has_data_available(connectfd, 10000)) {
            clear_and_init_message_container(&msg);

            if (read_message(connectfd, &msg) != 0) {
                perror("Cannot parse message, closing connection client");
                close(connectfd);
                continue;
            }

            // printf("=== request ===\n");
            //print_message(&msg);

            process_hashmap_operation(&msg);

            // printf("=== response ===\n");
            // print_message(&msg);


            if (send_message(connectfd, &msg) != 0) {
                perror("Error while sending");
            }
        }

        close(connectfd);
    }
    close(socketfd);
    freeaddrinfo(result);

    return 0;
}