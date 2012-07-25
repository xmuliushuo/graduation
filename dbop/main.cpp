#include "main.h"

int main()
{
    char buf[100];
    bzero(buf, 100);
    char ip[30] = "127.0.0.1";
    struct sockaddr_in servaddr;
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(10000);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	while (1) {
    	strcpy(buf, "insert into test(filename) values(100);\n");
    	Writen(sockfd, buf, strlen(buf));
		bzero(buf, 100);
		usleep(100);
	}
	int i;	
	scanf("%d", &i);
    return 0;
}
