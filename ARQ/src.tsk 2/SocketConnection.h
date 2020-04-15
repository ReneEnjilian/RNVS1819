#ifndef SOCKET_CONNECTION_H_
#define SOCKET_CONNECTION_H_

int udp_connect(const char *host, const char *serv);
int udp_server(const char *host, const char *serv, socklen_t *addrlenp);

#endif /* SOCKET_CONNECTION_H_ */
