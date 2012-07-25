/**
  * @brief project Client, 模拟了一个简单的客户端
  * @file main.cc
  * @author Liu Shuo
  * @date 2011.04.22
  */

#include "main.h"

struct sockaddr_in servaddr;
pthread_t pthread;
int count = 0;
int sendCount = 0;
pthread_mutex_t lock;
time_t start;
time_t now;
double times;
void *f(void *arg)
{
    Pthread_detach(pthread_self());
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    int rcount = rand() % 10;
    int num;
    char strnum[100];
    //char test[MAXLINE];
    string test("");
    for (int i = 0; i <= rcount; i++) {
        num = rand() % 10000;
        bzero(strnum, 100);
        sprintf(strnum, "%d", num);
        test.append(strnum);
        test = test + ",";
    }
    test[test.length() - 1] = '#';
    char recv[MAXLINE + 1] = {0};
    //sprintf(test, "%d", num);
    //cout << "send: " << test << endl;
    Sendto(sockfd, test.c_str(), test.length(), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Recvfrom(sockfd, recv, MAXLINE + 1, 0, NULL, NULL);
    //cout << "receve:";
    //cout << recv << endl;
    Pthread_mutex_lock(&lock);
    count++;
    //cout << count << endl;
    Pthread_mutex_unlock(&lock);
    Close(sockfd);
    return NULL;
}

void *calc(void *arg)
{
    start = time(NULL);
    int last = 0;
    while(1) {
        sleep(1);
        Pthread_mutex_lock(&lock);
        printf("send: %d recv: %d last seconde: %d\n", sendCount, count, count - last);
        last = count;
        Pthread_mutex_unlock(&lock);
    }
    return NULL;
}
/**
  * @brief 主函数
  */
int main()
{
    char ip[30] = "192.168.1.203";
    Pthread_mutex_init(&lock, NULL);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5002);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    Pthread_create(&pthread, NULL, calc, NULL);
    start = time(NULL);
    while (1) {
       //usleep(1000);
       //sleep(1);
	   sendCount++;
       pthread_create(&pthread, NULL, f, NULL);
    }
    return 0;
}
