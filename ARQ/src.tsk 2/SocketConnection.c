#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "SocketConnection.h"
#include "GoBackNMessageStruct.h"

/* Almost verbatim copies from the book "Unix Network Programming" */

int udp_connect(const char *host, const char *serv) {
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "udp_connect error for %s, %s: %s", host, serv,
            gai_strerror(n));
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) continue; /* ignore this one */

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break; /* success */

    close(sockfd); /* ignore this one */
  } while ((res = res->ai_next) != NULL);

  if (res == NULL) /* errno set from final connect() */ {
    fprintf(stderr, "udp_connect error for %s, %s", host, serv);
  }

  freeaddrinfo(ressave);

  return (sockfd);
}

int udp_server(const char *host, const char *serv, socklen_t *addrlenp) {
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    fprintf(stderr, "udp_server error for %s, %s: %s", host, serv,
            gai_strerror(n));
  }
  ressave = res;

  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
      continue; /* error - try next one */
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
      break; /* success */
    }

    close(sockfd); /* bind error - close and try next one */
  } while ((res = res->ai_next) != NULL);

  if (res == NULL) /* errno from final socket() or bind() */ {
    fprintf(stderr, "udp_server error for %s, %s", host, serv);
  }

  if (addrlenp) {
    *addrlenp = res->ai_addrlen; /* return size of protocol address */
  }

  freeaddrinfo(ressave);

  return (sockfd);
}
