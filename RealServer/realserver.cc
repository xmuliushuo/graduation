/**
  * @brief project RealServer, 实现了RealServer类的方法
  * @file realserver.cc
  * @author Liu Shuo
  * @date 2011.5.16
  */

#include "realserver.h"

/**
  * @brief 用于创建线程
  * param [in] arg: RealServer
  * @return NULL
  */
void *thread_heartbeat(void *arg)
{
#ifdef DEBUG
    cout << "执行函数thread_heartbeat(" << endl;
#endif
    return ((RealServer *)arg)->heartbeat(NULL);
}

/**
  * @brief 用于创建线程
  * param [in] arg: RealServer
  * @return NULL
  */
void *thread_serviceTCP(void *arg)
{
    return ((RealServer *)arg)->serviceTCP(NULL);
}

/**
  * @brief 用于创建线程
  * param [in] arg: point to struct requestArgs
  * @return NULL
  */
void *thread_handleRequest(void *arg)
{
    Pthread_detach(pthread_self());
    struct requestArgs *ptr = (struct requestArgs *)arg;
    ptr->realServer->handleRequest(ptr->connfd, &ptr->cliaddr, ptr->request);
    free(ptr->request);
    free(ptr);
    return (void *)NULL;
}

/**
  * @brief 用于创建线程
  * param [in] arg: RealServer
  * @return NULL
  */
void *thread_serviceUDP(void *arg)
{
    return ((RealServer *)arg)->serviceUDP(NULL);
}

/**
  * @brief 用于创建线程
  * param [in] arg: RealServer
  * @return NULL
  */
void *thread_dbUpdate(void *arg)
{
    return ((RealServer *)arg)->dbUpdate(NULL);
}

/**
  * @brief constructor
  */
RealServer::RealServer()
{
    m_connNum = 0;
    m_threadNum = 0;
    m_dbNum = 0;
    time_t now;
    time(&now);
    m_log = fopen("log", "a");
    fprintf(m_log, "\n%s: program started\n", ctime(&now));
    fclose(m_log);
    Pthread_mutex_init(&m_threadNumLock, NULL);
}

/**
  * @brief destructor
  */
RealServer::~RealServer()
{
    pthread_mutex_destroy(&m_threadNumLock);
}

/**
  * @brief 初始化节点
  * @class RealServer
  */
void RealServer::init()
{
    bzero(&serverMessageAddr, sizeof(serverMessageAddr));
    serverMessageAddr.sin_family = AF_INET;
    Inet_pton(AF_INET, m_lbIP, &serverMessageAddr.sin_addr);
    serverMessageAddr.sin_port = htons(m_lbMessagePort);
    if (m_serviceMode == 0){
        Pthread_create(&servicePthread, NULL, thread_serviceUDP, (void *)this);
    }
    else {
        Pthread_create(&servicePthread, NULL, thread_serviceTCP, (void *)this);
    }
    pthread_t tid;
    Pthread_create(&tid, NULL, thread_dbUpdate, (void *)this);
    usleep(1000);
    while (registerToTopNode() != 0) {
        sleep(5);
    }
    heartbeat(NULL);
    //Pthread_create(&heartbeatPthread, NULL, thread_heartbeat, (void *)this);
    //sleep(10000);
}

/**
  * @brief 向顶级节点注册
  * @class RealServer
  * @return 0 表示注册成功
  */
int RealServer::registerToTopNode()
{
#ifdef DEBUG
    cout << "执行函数registerToTopNode" << endl;
#endif
    int socketID = Socket(AF_INET, SOCK_STREAM, 0);
    char message[MAXLINE];
    sprintf(message, "%s,%s,%d,%d\n", "R", m_ID.c_str(), m_servicePort, m_property);
    Connect(socketID, (struct sockaddr *) &serverMessageAddr, sizeof(serverMessageAddr));
#ifdef DEBUG
    cout << "send: " << message << endl;
#endif
    Writen(socketID, message, strlen(message));
    Close(socketID);
    return 0;
}

/**
  * @brief 向负载均衡器发送心跳
  * @class RealServer
  * @param arg: NULL
  * @return NULL
  */
void *RealServer::heartbeat(void *arg)
{
#ifdef DEBUG
    cout << "execute heartbeat()" << endl;
#endif
    int socketID = Socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serverHeartBeatAddr;
    //初始化服务器端地址结构
    bzero(&serverHeartBeatAddr, sizeof(serverHeartBeatAddr));
    serverHeartBeatAddr.sin_family = AF_INET;
    Inet_pton(AF_INET, m_lbIP, &serverHeartBeatAddr.sin_addr);
    serverHeartBeatAddr.sin_port = htons(m_lbHbPort);

    //定时发送心跳
    string heartBeatMessage = "H,";
    heartBeatMessage += m_ID;
    int nowCount = 0;
    int lastCount = 0;
    int nowdbCount= 0;
    int lastdbCount = 0;
    while(1) {
        //cout << "send heartbeat " << heartBeatMessage << endl;
        //printf("处理请求%d\n", m_connNum);
        time_t now;
        time(&now);
        nowCount = m_connNum;
        nowdbCount = m_dbNum;
        m_log = fopen("log", "a");
        fprintf(m_log, "%s: all request: %d; the last 1 second request: %d; db options: %d\n", ctime(&now), nowCount, nowCount - lastCount, nowdbCount - lastdbCount);
        fclose(m_log);
        lastCount = nowCount;
        lastdbCount = nowdbCount;
        Sendto(socketID, heartBeatMessage.c_str(), heartBeatMessage.length(), 0,
            (struct sockaddr *) &serverHeartBeatAddr, sizeof(serverHeartBeatAddr));
        sleep(1);
    }
    return (void *)NULL;
}

/**
  * @brief 提供TCP服务
  * @class RealServer
  * @param arg: NULL
  * @return NULL
  */
void *RealServer::serviceTCP(void *arg)
{
    int listenfd, connfd;
    pthread_t tid;
    socklen_t clilen;
    struct sockaddr_in serveraddr, cliaddr;
    struct requestArgs *args;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_servicePort);

    Bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    Listen(listenfd, 1024);
    while (1) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        args = (struct requestArgs *)Malloc(sizeof(struct requestArgs));
        args->realServer = this;
        args->connfd = connfd;
        Pthread_create(&tid, NULL, thread_handleRequest, (void *)args);
    }
    return (void *)NULL;
}

/**
  * @brief 提供UDP服务
  * @class RealServer
  * @param arg: NULL
  * @return NULL
  */
void *RealServer::serviceUDP(void *arg)
{
    m_sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serveraddr;
    char buf[MAXLINE] = {0};
    string IP, port, filename;
    string temp;
    pthread_t tid;

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_servicePort);
    Bind(m_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    while (1) {
        bzero(buf, MAXLINE);
        Recvfrom(m_sockfd, buf, MAXLINE, 0, NULL, NULL);
        m_connNum++;
        temp.assign(buf);
        int i = 0;
        IP = "";
        port = "";
        filename = "";
        while (temp[i] != ',')
        {
            IP.push_back(temp[i]);
            i++;
        }
        i++;

        while (temp[i] != ',')
        {
            port.push_back(temp[i]);
            i++;
        }
        i++;
        while (temp[i] != '\n') {
            filename.push_back(temp[i]);
            i++;
        }
        //cout << IP << ":" << port << endl;
        //cout << filename << endl;

        struct requestArgs *args;
        args = (struct requestArgs *)Malloc(sizeof(struct requestArgs));
        args->request = new string();
        args->request->assign(filename);
        args->realServer = this;
        bzero(&args->cliaddr, sizeof(args->cliaddr));
        args->cliaddr.sin_family = AF_INET;
        Inet_pton(AF_INET, IP.c_str(), &args->cliaddr.sin_addr);
        args->cliaddr.sin_port = htons(atoi(port.c_str()));

        if (pthread_create(&tid, NULL, thread_handleRequest, (void *)args) != 0) {
            //Sendto(m_sockfd, mesg, strlen(mesg), 0, (struct sockaddr *)&(args->cliaddr), clilen);
            //printf("pthread_error\n");
            handleRequest(0, &args->cliaddr, args->request);
            free(args->request);
            free(args);
        }
    }
    return NULL;
}

/**
  * @brief 处理发送过来的请求
  * @class RealServer
  * @param [in] connfd 请求套接字
  */
void *RealServer::handleRequest(int connfd, const struct sockaddr_in *cliaddr, string *request)
{
    /*
    int num = atoi(request->c_str());
    sprintf(sql, "%s%d%s", "select ip from test where filename=", num, " limit 1");
    //cout << sql << endl;
    MYSQL *mysql = m_pool->getOneConn();
    if (mysql != NULL) {
        MYSQL_ROW row;
        MYSQL_RES *result;
        if (mysql_query(mysql, sql) != 0) {
            cout << "mysql query error" << endl;
            exit(1);
        }
        if ((result = mysql_store_result(mysql)) == NULL) {
            cout << "mysql store result error" << endl;
            exit(1);
        }
        if ((row = mysql_fetch_row(result))) {
            sprintf(buf, "%s", row[0]);
        }
        else {
            sprintf(buf, "%s", "NULL");
        }
        mysql_free_result(result);
        m_pool->retOneConn(mysql);
    }
    else {
        cout << "mysql连接数不足" << endl;
        sprintf(buf, "%s", "ERROR");
    }*/
    char buf[MAXLINE] = {0};
    char strresult[MAXLINE] = {0};
    char sql[MAXLINE] = {0};
    int num[20];
    string temp;
    int i = 0;
    int j = 0;
    while (1) {
        while ((*request)[i] != ',' && (*request)[i] != '#') {
            temp.push_back((*request)[i]);
            i++;
        }
        //cout << temp << endl;
        num[j] = atoi(temp.c_str());
        temp.assign("");
        j++;
        i++;
        if ((*request)[i - 1] == '#') {
            num[j] = -1;
            break;
        }
    }
    //cout << "handle request " << *request << endl;
#ifdef DEBUG
    //cout << "client information: " << endl;
    //cout << "ip: " << Inet_ntop(AF_INET, &cliaddr->sin_addr, buf, sizeof(buf)) << endl;
    //cout << "port: " << ntohs(cliaddr->sin_port) << endl;
#endif

    j = 0;
    MYSQL *mysql = m_pool->getOneConn();
    while (1) {
        bzero(buf, MAXLINE);
        if (num[j] == -1)
            break;
        sprintf(sql, "%s%d%s", "select ip from test where filename=", num[j], " limit 1");
        //cout << sql << endl;
        if (mysql != NULL) {
            MYSQL_RES *result;
            MYSQL_ROW row;
            if (mysql_query(mysql, sql) != 0) {
                cout << "mysql query error" << endl;
                exit(1);
            }
            if ((result = mysql_store_result(mysql)) == NULL) {
                cout << "mysql store result error" << endl;
                exit(1);
            }
            if ((row = mysql_fetch_row(result))) {
                sprintf(buf, "%s,", row[0]);
            }
            else {
                sprintf(buf, "%s,", "NULL");
            }
            strcat(strresult, buf);
            mysql_free_result(result);
        }
        else {
            cout << "mysql连接数不足" << endl;
            sprintf(buf, "%s", "ERROR");
        }
        j++;
    }
    m_pool->retOneConn(mysql);
    //cout << strresult << endl;
    //Sendto(m_sockfd, buf, strlen(buf), 0, (struct sockaddr *)cliaddr, sizeof(*cliaddr));
    Sendto(m_sockfd, strresult, strlen(strresult), 0, (struct sockaddr *)cliaddr, sizeof(*cliaddr));

    //cout << buf << endl;
    //Sendto(m_sockfd, buf, strlen(buf), 0, (struct sockaddr *)cliaddr, sizeof(*cliaddr));
    return NULL;
}

/**
  * @brief 更新数据库
  * @class RealServer
  * @param [in] arg: NULL
  * @return NULL
  */
void *RealServer::dbUpdate(void *arg)
{
    struct sockaddr_in dbaddr;
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    char buf[MAXLINE];

    bzero(&dbaddr, sizeof(dbaddr));
    dbaddr.sin_family = AF_INET;
    dbaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    dbaddr.sin_port = htons(m_dbPort);
    Bind(sockfd, (struct sockaddr *)&dbaddr, sizeof(dbaddr));
    Listen(sockfd, 10);
    MYSQL *mysql = m_pool->getOneConn();
    if (mysql == NULL) {
        cout << "mysql连接数不足" << endl;
        exit(1);
    }
    while (1) {
        int connfd = Accept(sockfd, NULL, NULL);
        //cout << "accepted" << endl;
        while (readline(connfd, buf, MAXLINE) > 0) {
            //cout << "received" << endl;
            //printf("%s", buf);
            m_dbNum++;
            if (mysql_query(mysql, buf) != 0) {
                cout << "mysql query error" << endl;
                //exit(1);
            }
            bzero(buf, MAXLINE);
        }
        //cout << "end" << endl;
    }
    return NULL;
}

/**
  * @class RealServer
  * @brief 读取配置文件
  * @param [in] filename: 配置文件地址
  */
void RealServer::readConf(char *filename)
{
    FILE *fp = Fopen(filename, "r");
    int num = 0;
    char buf[MAXLINE];
    char dbip[100];
    char dbUser[100];
    char dbPasswd[100];
    char dbName[100];
    int dbPort;
    int dbConnNum;

    while (Fgets(buf, MAXLINE, fp) != NULL) {
#ifdef DEBUG
        //cout << buf;
#endif
        if (buf[0] != '#') {
            buf[strlen(buf) - 1] = 0;
            switch (num) {
            case 0:
                strcpy(m_lbIP, buf);
                break;
            case 1:
                m_lbHbPort = atoi(buf);
                break;
            case 2:
                m_lbMessagePort = atoi(buf);
                break;
            case 3:
                m_ID.assign(buf);
                break;
            case 4:
                m_servicePort = atoi(buf);
                break;
            case 5:
                strcpy(dbip, buf);
                break;
            case 6:
                strcpy(dbUser, buf);
                break;
            case 7:
                strcpy(dbPasswd, buf);
                break;
            case 8:
                strcpy(dbName, buf);
                break;
            case 9:
                dbPort = atoi(buf);
                break;
            case 10:
                dbConnNum = atoi(buf);
                break;
            case 11:
                m_property = atoi(buf);
                break;
            case 12:
                m_dbPort = atoi(buf);
                break;
            default:
                break;
            }
            num++;
        }
    }
#ifdef DEBUG
    cout << "the load balancer IP: " << m_lbIP << endl;
    cout << "the load balancer heartbeat port: " << m_lbHbPort << endl;
    cout << "the load balancer message port: " << m_lbMessagePort << endl;
    cout << "node ID: " << m_ID << endl;
    cout << "the service port: " << m_servicePort << endl;
    cout << "mysql ip: " << dbip << endl;
    cout << "mysql user: " << dbUser << endl;
    cout << "mysql passwd: " << dbPasswd << endl;
    cout << "mysql database name: " << dbName << endl;
    cout << "mysql port: " << dbPort << endl;
    cout << "mysql connection num: " << dbConnNum << endl;
    cout << "server property: " << m_property << endl;
#endif
    m_pool = new ConnPool();
    m_pool->init(dbip, dbUser, dbPasswd, dbName, dbPort, dbConnNum);
    m_serviceMode = 0;
    m_maxThreadNum = 2;
    m_dbPort = m_servicePort + 1;
}
