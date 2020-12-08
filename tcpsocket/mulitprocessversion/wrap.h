#ifndef _WRAP_H_
#define _WRAP_H_
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>
void geterror(char *s);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen;
int Listen(int sockfd, int backlog); 
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


#endif
