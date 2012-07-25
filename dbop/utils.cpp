
/**
  * @brief project TopNode, 包装了部分库函数
  * @file utils.cc
  * @author Liu Shuo
  * @date 2011.04.18
  */

#include "main.h"

/**
  * @brief 包装了库函数accept
  */
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int n;
again:
    if ((n = accept(fd, sa, salenptr)) < 0)
    {
#ifdef	EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else
        {
            //err_sys("accept error");
            cout << "accept error" << endl;
            exit(1);
        }
    }
    return n;
}

/**
  * @brief 包装了库函数pthread_create
  */
void Pthread_create(pthread_t *tid, const pthread_attr_t *attr, void * (*func)(void *), void *arg)
{
    int	n;
    if ((n = pthread_create(tid, attr, func, arg)) == 0)
        return;
    errno = n;
    //err_sys("pthread_create error");
    cout << "pthread_create error" << endl;
    exit(1);
}

/**
  * @brief 包装了库函数Socket
  */
int Socket(int family, int type, int protocol)
{
    int n;
    if ((n = socket(family, type, protocol)) < 0)
    {
        //err_sys("socket error");
        cout << "socket error" << endl;
        exit(1);
    }
    return n;
}

/**
  * @brief 包装了库函数bind
  */
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
    {
        //err_sys("bind error");
        cout << "bind error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数pthread_mutex_init
  */
void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
    int	n;
    if ((n = pthread_mutex_init(mptr, attr)) == 0)
        return;
    errno = n;
    //err_sys("pthread_mutex_init error");
    cout << "pthread_mutex_init error" << endl;
    exit(1);
}

/**
  * @brief 包装了库函数pthread_mutex_lock
  */
void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
    int	n;
    if ((n = pthread_mutex_lock(mptr)) == 0)
        return;
    errno = n;
    //err_sys("pthread_mutex_lock error");
    cout << "pthread_mutex_lock error" << endl;
    exit(1);
}

/**
  * @brief 包装了库函数pthread_mutex_unlock
  */
void Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
    int	n;
    if ((n = pthread_mutex_unlock(mptr)) == 0)
        return;
    errno = n;
    //err_sys("pthread_mutex_unlock error");
    cout << "pthread_mutex_unlock error" << endl;
    exit(1);
}

/**
  * @brief 包装了库函数recvfrom
  */
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr)
{
    ssize_t	n;
    if ((n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
    {
        //err_sys("recvfrom error");
        cout << "recvfrom error" << endl;
        exit(1);
    }
    return n;
}

/**
  * @brief 包装了库函数listen
  */
void Listen(int fd, int backlog)
{
    //char *ptr;
    if (listen(fd, backlog) < 0)
    {
        //err_sys("listen error");
        cout << "listen error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数pthread_detach
  */
void Pthread_detach(pthread_t tid)
{
    int	n;
    if ((n = pthread_detach(tid)) == 0)
        return;
    errno = n;
    //err_sys("pthread_detach error");
    cout << "pthread_detach error" << endl;
    exit(1);
}

/**
  * @brief 包装了库函数close
  */
void Close(int fd)
{
    if (close(fd) == -1)
    {
        //err_sys("close error");
        cout << "close error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数malloc
  */
void *Malloc(size_t size)
{
    void *ptr;
    if ((ptr = malloc(size)) == NULL)
    {
        //err_sys("malloc error");
        cout << "malloc error" << endl;
        exit(1);
    }
    return(ptr);
}

/**
  * @brief 包装了库函数inet_ntop
  */
const char *Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
    const char *ptr;
    if (strptr == NULL)  		/* check for old code */
    {
        //err_quit("NULL 3rd argument to inet_ntop");
        cout << "NULL 3rd argument to inet_ntop" << endl;
        exit(1);
    }
    if ((ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
    {
        //err_sys("inet_ntop error");		/* sets errno */
        cout << "inet_ntop error" << endl;
        exit(1);
    }
    return(ptr);
}

/**
  * @brief 包装了库函数read
  */
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t	n;
    if ((n = read(fd, ptr, nbytes)) == -1)
    {
        //err_sys("read error");
        cout << "read error" << endl;
        exit(1);
    }
    return(n);
}

/**
  * @brief 包装了库函数connect
  */
void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (connect(fd, sa, salen) < 0)
    {
        //err_sys("connect error");
        cout << "connect error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数write
  */
void Write(int fd, const void *ptr, size_t nbytes)
{
    if (write(fd, ptr, nbytes) != (int)nbytes)
    {
        //err_sys("write error");
        cout << "write error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数open
  */
int Open(const char *pathname, int oflag, mode_t mode)
{
    int	fd;
    if ((fd = open(pathname, oflag, mode)) == -1)
    {
        cout << "open error" << endl;
        exit(1);
        //err_sys("open error for %s", pathname);
    }
    return(fd);
}

/**
  * @brief 包装了库函数fopen
  */
FILE * Fopen(const char *filename, const char *mode)
{
    FILE *fp;
    if ((fp = fopen(filename, mode)) == NULL)
    {
        cout << "fopen error" << endl;
        exit(1);
        //err_sys("fopen error");
    }
    return(fp);
}

/**
  * @brief 包装了库函数fgets
  */
char *Fgets(char *ptr, int n, FILE *stream)
{
    char *rptr;
    rptr = fgets(ptr, n, stream);
    if (ferror(stream))
    {
        cout << "fgets error" << endl;
        exit(1);
        //err_sys("fgets error");
    }
    return (rptr);
}

/**
  * @brief Write "n" bytes to a descriptor.
  * @param [in] fd
  * @param [in] vptr
  * @param [in] n
  * @return
  */
ssize_t	writen(int fd, const void *vptr, size_t n)
{
    size_t		nleft;
    ssize_t		nwritten;
    const char	*ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;		/* and call write() again */
            else
                return(-1);			/* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

/**
  * @brief 包装了函数writen
  */
void Writen(int fd, void *ptr, size_t nbytes)
{
    if ((size_t)writen(fd, ptr, nbytes) != nbytes)
    {
        cout << "writen error" << endl;
        exit(1);
    }
    //err_sys("writen error");
}

/**
  * @brief 包装了库函数sendto
  */
void Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen)
{
    if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
    {
        //err_sys("sendto error");
        cout << "sendto error" << endl;
        exit(1);
    }
}

/**
  * @brief 包装了库函数inet_pton
  */
void Inet_pton(int family, const char *strptr, void *addrptr)
{
    int	n;
    if ((n = inet_pton(family, strptr, addrptr)) <= 0)
    {
        cout << "inet_pton error" << endl;
        exit(1);
    }
}
