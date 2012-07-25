
/**
  * @brief project TopNode, 定义了工程用到的头文件，常量，函数等
  * @file main.h
  * @author Liu Shuo
  * @date 2011.4.18
  */

#ifndef MAIN
#define MAIN

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <mysql/mysql.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>

#define MAXLINE 1000
#define MAXCONNUM 1024
#define DEBUG
#define LIVE 0
#define DEAD 1

using namespace std;

int     Accept(int, struct sockaddr *, socklen_t *);
void    Bind(int, const struct sockaddr *, socklen_t);
void    Close(int);
void    Connect(int, const struct sockaddr *, socklen_t);
char    *Fgets(char *, int, FILE *);
const char *Inet_ntop(int, const void *, char *, size_t);
void    Listen(int, int);
void    *Malloc(size_t);
void    Pthread_create(pthread_t *, const pthread_attr_t *, void * (*)(void *), void *);
int	    Socket(int, int, int);
void    Pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void    Pthread_mutex_lock(pthread_mutex_t *);
void    Pthread_mutex_unlock(pthread_mutex_t *);
ssize_t Recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
void    Pthread_detach(pthread_t);
ssize_t Read(int, void *, size_t);
void    Sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
void    Write(int, const void *, size_t);
ssize_t	writen(int, const void *, size_t);
void    Writen(int, void *, size_t);
int     Open(const char *, int, mode_t);
FILE    *Fopen(const char *, const char *);
void    Inet_pton(int, const char *, void *);

#endif
