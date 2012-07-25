/**
  * @brief project TopNode,
  * @file topnode.cc
  * @author Liu Shuo
  * @date 2011.4.18
  */

#include "topnode.h"
#include "node.h"

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: NULL
  * @return NULL
  */
void *thread_recvhearbeat(void *arg)
{
    return ((TopNode *)arg)->receiveheartbeat(NULL);
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: NULL
  * @return NULL
  */
void *thread_recvmessage(void *arg)
{
    return ((TopNode *)arg)->receiveMessage(NULL);
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: NULL
  * @return NULL
  */
void *thread_changeStatus(void *arg)
{
    return ((TopNode *)arg)->changeStatus(NULL);
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: NULL
  * @return NULL
  */
void *thread_serve(void *arg)
{
    return ((TopNode *)arg)->serve(NULL);
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: struct request Args *
  * @return NULL
  */
void *thread_handleRequest(void *arg)
{
    Pthread_detach(pthread_self());
    struct requestArgs *ptr = (struct requestArgs *)arg;
    ptr->topNode->handleRequest(ptr->connfd, &ptr->cliaddr, ptr->request);
    free(ptr->request);
    free(ptr);
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: struct request Args *
  * @return NULL
  */
void *thread_handleMesg(void *arg)
{
    Pthread_detach(pthread_self());
    struct requestArgs *ptr = (struct requestArgs *)arg;
    ptr->topNode->handleMesg(ptr->connfd, &ptr->cliaddr);
    free(arg);
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 用于创建线程
  * @param [in] arg: struct request Args *
  * @return NULL
  */
void *thread_handledbUpdate(void *arg)
{
    Pthread_detach(pthread_self());
    struct requestArgs *ptr = (struct requestArgs *)arg;
    ptr->topNode->handledbUpdate(ptr->connfd);
    free(arg);
    return (void *)NULL;
}

void *thread_dbUpdate(void *arg)
{
    return ((TopNode *)arg)->dbUpdate(NULL);
}

/**
  * @class TopNode
  * @brief 构造函数
  */
TopNode::TopNode()
{
    m_nextNode = 0;
    m_count = 0;
    m_rsNum = 0;
    m_threadNum = 0;
    m_dbPort = 10000;
    //m_log = open("log", O_WRONLY, O_APPEND);
    Pthread_mutex_init(&m_nextNodeLock, NULL);
    Pthread_mutex_init(&m_propertyLock, NULL);
    Pthread_mutex_init(&m_threadNumLock, NULL);
    Pthread_mutex_init(&m_dbUpdateLock, NULL);
}

/**
  * @class TopNode
  * @brief 析构函数
  */
TopNode::~TopNode()
{
    pthread_mutex_destroy(&m_nextNodeLock);
    pthread_mutex_destroy(&m_propertyLock);
    pthread_mutex_destroy(&m_threadNumLock);
    pthread_mutex_destroy(&m_dbUpdateLock);
    delete m_pool;
    for (int i = 0; i < nodeVector.size(); i++) {
        delete nodeVector[i];
    }
}

/**
  * @class TopNode
  * @brief 初始化负载均衡服务器
  */
void TopNode::init()
{
    time_t now;
    time(&now);
    m_log = fopen("log", "a");
    fprintf(m_log, "\n%s: program started\n", ctime(&now));
    fclose(m_log);
    Pthread_create(&heatbeatPthread, NULL, thread_recvhearbeat, (void *)this);
    Pthread_create(&messagePthread, NULL, thread_recvmessage, (void *)this);
    Pthread_create(&statusPthread, NULL, thread_changeStatus, (void *)this);
    Pthread_create(&m_dbPthread, NULL, thread_dbUpdate, (void *)this);
    Pthread_create(&servePthread, NULL, thread_serve, (void *)this);
    console();
}

/**
  * @class TopNode
  * @brief 初始化负载均衡服务器
  * @param [in] arg: NULL
  * @return NULL
  */
void *TopNode::receiveheartbeat(void *arg)
{
#ifdef DEBUG
    cout << "execute receiveheartbeat()" << endl;
#endif
    int socketID = Socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_heartbeatPort);

    Bind(socketID, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    char message[MAXLINE];
    socklen_t len;
    while (1)
    {
        Recvfrom(socketID, message, MAXLINE, 0, (struct sockaddr *) &clientaddr, &len);
#ifdef DEBUG
        //cout << message << endl;
#endif
        int i = 2;
        string nodeID("");
        while (message[i] != 0)
        {
            nodeID.push_back(message[i]);
            i++;
        }
        for (vector<Node *>::size_type i = 0; i != nodeVector.size(); i++)
        {
            if (nodeID == nodeVector[i]->getID())
            {
                nodeVector[i]->setheartBeatTime(time(NULL));
                nodeVector[i]->setstatus(LIVE);
#ifdef DEBUG
                cout << nodeVector[i]->getID() << " heart beat time: "
                     << nodeVector[i]->getheartBeatTime() << endl;
#endif
                break;
            }
        }
    }
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 接收来自节点的消息
  * @param arg is NULL
  * @return NULL
  */
void *TopNode::receiveMessage(void *arg)
{
#ifdef DEBUG
    cout << "execute receiveMessage" << endl;
#endif
    int socketID, connfd;
    socklen_t clilen;
    socketID = Socket(AF_INET, SOCK_STREAM, 0);
    pthread_t tid;
    struct requestArgs* args;
    struct sockaddr_in serveraddr;

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_messagePort);
    Bind(socketID, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    Listen(socketID, 10);
    while (1)
    {
        args = (struct requestArgs *)Malloc(sizeof(struct requestArgs));
        clilen = sizeof(args->cliaddr);
        connfd = Accept(socketID, (struct sockaddr *)&(args->cliaddr), &clilen);
        args->topNode = this;
        args->connfd = connfd;
        Pthread_create(&tid, NULL, thread_handleMesg, args);
    }
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 检查节点状态
  * @param arg: NULL
  * @return NULL
  */
void *TopNode::changeStatus(void *arg)
{
    int lastCount = 0;
    int nowCount = 0;
    while (1)
    {
        sleep(1);
        time_t now;
        time(&now);
        nowCount = m_count;
        m_log = fopen("log", "a");
        fprintf(m_log, "%s: all request: %d; the last 1 second request: %d\n", ctime(&now), nowCount, nowCount - lastCount);
        fclose(m_log);
        lastCount = nowCount;
        //printf("处理请求：%d\n", m_count);
        for (vector<Node *>::size_type i = 0; i != nodeVector.size(); i++)
        {
            if (nodeVector[i]->getstatus() == LIVE)
            {
                if ((time(NULL) - nodeVector[i]->getheartBeatTime()) > 3)
                {
                    cout << "node " << nodeVector[i]->getID() << " is dead!" << endl;
                    nodeVector[i]->setstatus(DEAD);
                    //nodeVector[i]->closeServer();
                }
            }
        }
    }
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 对外提供服务
  * @param arg: NULL
  * @return NULL
  */
void *TopNode::serve(void *arg)
{
    m_sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    int len;
    char mesg[MAXLINE];
    struct sockaddr_in serveraddr;
    //struct sockaddr_in s_cliaddr;
    socklen_t clilen;
    pthread_t tid;
    struct requestArgs *args;

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_servicePort);
    Bind(m_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    while (1) {
        bzero(mesg, MAXLINE);
        args = (struct requestArgs *)Malloc(sizeof(struct requestArgs));
        clilen = sizeof(args->cliaddr);
        len = Recvfrom(m_sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&(args->cliaddr), &clilen);
        args->request = new string();
        //cout << mesg << endl;
        args->request->assign(mesg);
        //cout << "receive: " << *args->request << endl;
        args->topNode = this;
        m_count++;

        if (pthread_create(&tid, NULL, thread_handleRequest, (void *)args) != 0) {
            //Sendto(m_sockfd, mesg, strlen(mesg), 0, (struct sockaddr *)&(args->cliaddr), clilen);
            //printf("pthread_error\n");
            handleRequest(0, &args->cliaddr, args->request);
            free(args->request);
            free(args);
        }
    }
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief handle the request from client
  * @param [in] connfd: the socket ID of client
  * @param [in] cliaddr: 客户端地址结构
  */
void *TopNode::handleRequest(int connfd, const struct sockaddr_in *cliaddr, string *request)
{
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
    if (m_policy == 0) {//不转发
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
    }
    else{
        sprintf(buf, "%s,%d,%s\n",
            Inet_ntop(AF_INET, &cliaddr->sin_addr, buf, sizeof(buf)), ntohs(cliaddr->sin_port), request->c_str());
        Node *node;
        switch (m_policy) {
            case 1:
                node = getNextNode();
                break;
            case 2:
                node = getNextNodeByIP(cliaddr->sin_addr.s_addr);
                break;
            case 3:
                //node = getNextNodeByReq(num);
                break;
            case 4:
                node = getNextNodeByLoad();
                break;
            case 5:
                node = getNextNodeByProperty();
                break;
            default:
                node = NULL;
                break;
        }
        if (node == NULL) {
            cout << "there is no real server!" << endl;
            exit(1);
        }
        //cout << buf << endl;
        node->sendtoRS(m_sockfd, buf, strlen(buf));
    }
    return (void *)NULL;
}

/**
  * @class TopNode
  * @brief 处理接收的消息
  * @param [in] connfd: the socket ID of client
  * @param [in] cliaddr: 客户端地址结构
  */
void *TopNode::handleMesg(int connfd, const struct sockaddr_in *cliaddr)
{
    char buf[MAXLINE] = {0};
    string mesg = "";
    int i;

    while (read(connfd, buf, MAXLINE) > 0)
    {
        cout << buf << endl;
        mesg.append(buf);
        bzero(buf, MAXLINE);
    }
    switch (mesg[0])
    {
        case 'R': { //注册新节点, 故障节点恢复
            i = 2;
            string nodeID("");
            string strPort("");
            string strProperty("");
            while (mesg[i] != ',')
            {
                nodeID.push_back(mesg[i]);
                i++;
            }
            i++;
            for (int j = 0; j < nodeVector.size(); j++)
            {
                if (nodeVector[j]->getID() == nodeID) {
                    printf("节点%s恢复\n", nodeID.c_str());
                    nodeVector[j]->setstatus(LIVE);
                    Close(connfd);
                    return NULL;
                }
            }
            while (mesg[i] != ',')
            {
                strPort.push_back(mesg[i]);
                i++;
            }
            i++;
            while (mesg[i] != '\n') {
                strProperty.push_back(mesg[i]);
                i++;
            }
#ifdef DEBUG
            cout << "register new node, node ID: "<< nodeID << endl;
            cout << "node service port: " << strPort << endl;
            cout << "node property: " << strProperty << endl;
#endif
            Node *newNode = new Node(nodeID, strPort, cliaddr->sin_addr.s_addr, atoi(strProperty.c_str()));
            newNode->setstatus(LIVE);
            nodeVector.push_back(newNode);
            if (m_policy != 0) {
                newNode->connecttoDB();
            }
            m_rsNum++;
            initProperty();
            break;
        }
        default:
            break;
    }
    Close(connfd);
    return NULL;
}

/**
  * @class TopNode
  * @brief 获得下一个可用的节点指针
  * @return 节点指针
  */
Node *TopNode::getNextNode()
{
    Pthread_mutex_lock(&m_nextNodeLock);
    while (1)
    {
        if (m_nextNode >= nodeVector.size())
        {
            m_nextNode = 0;
        }
        if (nodeVector[m_nextNode]->getstatus() == LIVE) {
            Node *result = nodeVector[m_nextNode];
            m_nextNode++;
			Pthread_mutex_unlock(&m_nextNodeLock);
            return result;
        }
        m_nextNode++;
    }
}

/**
  * @class TopNode
  * @brief 通过ip hash,获得下一个可用的节点指针
  * @param [in] ip: 客户端ip
  * @return 节点指针
  */
Node *TopNode::getNextNodeByIP(uint32_t ip)
{
    uint32_t index;
    Node *result;
    if (m_rsNum != 0) {
		index = (ip * 2654435761) & 0x00FF;
        //index = ip % m_rsNum;
	}
    else {
        return NULL;
    }
	index = index % rsNum;
    if (nodeVector[index]->getstatus() == LIVE) {
        result = nodeVector[index];
        return result;
    }
    else {
        return getNextNode();
    }
    return getNextNode();
}

/**
  * @class TopNode
  * @brief 通过请求内容hash,获得下一个可用的节点指针
  * @param [in] num: 请求内容
  * @return 节点指针
  */
Node *TopNode::getNextNodeByReq(int num)
{
    int index;
    if (m_rsNum != 0)
        index = num % m_rsNum;
    else {
        return NULL;
    }
    if (nodeVector[index]->getstatus() == LIVE) {
        return nodeVector[index];
    }
    else {
        return getNextNode();
    }
    return getNextNode();
}

/**
  * @class TopNode
  * @brief 通过服务器的负载情况，获得下一个可用的节点指针
  * @return 节点指针
  */
Node *TopNode::getNextNodeByLoad()
{
    //暂时未实现
    return NULL;
}

/**
  * @class TopNode
  * @brief 通过服务器的优先级，获得下一个可用的节点指针
  * @return 节点指针
  */
Node *TopNode::getNextNodeByProperty()
{
    //printf("getNextNodeByProperty\n");
    Pthread_mutex_lock(&m_propertyLock);
    int index = m_count % m_propertySum;
    //printf("%d\n", index);
    for (vector<Node *>::size_type i = 0; i != nodeVector.size(); i++) {
        if (index >= nodeVector[i]->m_sProMesg.min && index <= nodeVector[i]->m_sProMesg.max) {
            if (nodeVector[i]->getstatus() == LIVE) {
                Pthread_mutex_unlock(&m_propertyLock);
                return nodeVector[i];
            }
            else {
                Pthread_mutex_unlock(&m_propertyLock);
                return getNextNode();
            }
        }
    }
    Pthread_mutex_unlock(&m_propertyLock);
    return getNextNode();
}

/**
  * @class TopNode
  * @brief 初始化服务器优先级
  */
void TopNode::initProperty()
{
    Pthread_mutex_lock(&m_propertyLock);
    vector<Node *>::size_type i = 0;
    m_propertySum = 0;
    for (i = 0; i != nodeVector.size(); i++) {
        nodeVector[i]->m_sProMesg.min = m_propertySum;
        m_propertySum += nodeVector[i]->m_sProMesg.property;
        nodeVector[i]->m_sProMesg.max = m_propertySum - 1;
    }
    Pthread_mutex_unlock(&m_propertyLock);
}

/**
  * @class TopNode
  * @brief 读取配置文件
  * @param [in] 配置文件地址
  */
void TopNode::readConf(char *filename)
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

    while (Fgets(buf, MAXLINE, fp) != NULL)
    {
        if (buf[0] != '#')
        {
            buf[strlen(buf) - 1] = 0;
            switch (num)
            {
            case 0:
                m_heartbeatPort = atoi(buf);
                break;
            case 1:
                m_messagePort = atoi(buf);
                break;
            case 2:
                m_servicePort = atoi(buf);
                break;
            case 3:
                strcpy(dbip, buf);
                break;
            case 4:
                strcpy(dbUser, buf);
                break;
            case 5:
                strcpy(dbPasswd, buf);
                break;
            case 6:
                strcpy(dbName, buf);
                break;
            case 7:
                dbPort = atoi(buf);
                break;
            case 8:
                dbConnNum = atoi(buf);
                break;
            case 9:
                m_policy = atoi(buf);
                break;
            case 10:
                m_maxThreadNum = atoi(buf);
                break;
            default:
                break;
            }
            num++;
        }
    }
#ifdef DEBUG
    cout << "heartbeat port: " << m_heartbeatPort << endl;
    cout << "message port: " << m_messagePort << endl;
    cout << "service port: " << m_servicePort << endl;
    cout << "mysql ip: " << dbip << endl;
    cout << "mysql user: " << dbUser << endl;
    cout << "mysql passwd: " << dbPasswd << endl;
    cout << "mysql database name: " << dbName << endl;
    cout << "mysql port: " << dbPort << endl;
    cout << "mysql connection num: " << dbConnNum << endl;
#endif
    m_pool = new ConnPool();
    m_pool->init(dbip, dbUser, dbPasswd, dbName, dbPort, dbConnNum);
}

void *TopNode::dbUpdate(void *arg)
{
    //cout << "dbupdate" << endl;
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr, cliaddr;
    int connfd;
    socklen_t clilen = sizeof(cliaddr);
    struct requestArgs* args;
    pthread_t tid;

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(m_dbPort);

    Bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    Listen(sockfd, 10);

    while (1) {
        args = (struct requestArgs *)Malloc(sizeof(struct requestArgs));
        connfd = Accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        args->topNode = this;
        args->connfd = connfd;
        Pthread_create(&tid, NULL, thread_handledbUpdate, args);
    }
    return NULL;
}

void *TopNode::handledbUpdate(int connfd)
{
    char buf[MAXLINE];
    bzero(buf, MAXLINE);
    struct timeval timeout={2, 0};
    while (read(connfd, buf, MAXLINE) > 0) {
        Pthread_mutex_lock(&m_dbUpdateLock);
        fd_set wset;
        for (int i = 0; i < nodeVector.size(); i++) {
            if (nodeVector[i]->getstatus() == LIVE) {
                FD_ZERO(&wset);
                FD_SET(nodeVector[i]->m_sockdbfd, &wset);
            }
            select(nodeVector[i]->m_sockdbfd + 1, NULL, &wset, NULL, &timeout);
        }
        for (int i = 0; i < nodeVector.size(); i++) {
            if (nodeVector[i]->getstatus() == LIVE) {
                //cout << "send to" << nodeVector[i]->m_sockdbfd << endl;
                //printf("%s", buf);
                if (send(nodeVector[i]->m_sockdbfd, buf, strlen(buf), MSG_DONTWAIT) < 0) {
                    printf("send too much data!\n");
                }
            }
        }
        Pthread_mutex_unlock(&m_dbUpdateLock);
        bzero(buf, MAXLINE);
    }
    cout << "close" << endl;
    Close(connfd);
    return NULL;
}

void TopNode::console()
{
    char cmd;
    for ( ; ;) {
        cmd = getchar();
        switch (cmd) {
            case 'h':
                printf("h -- for help.\n");
                printf("q -- exit the program.\n");
                break;
            case 'q':
                printf("clean...\n");
                exitLB();
                printf("clean completed\n");
                goto next;
                break;
            default:
                break;
        }
    }
next: return;
}

void TopNode::exitLB()
{
    pthread_cancel(heatbeatPthread);
    pthread_cancel(messagePthread);
    pthread_cancel(statusPthread);
    pthread_cancel(servePthread);
    pthread_cancel(m_dbPthread);
}
