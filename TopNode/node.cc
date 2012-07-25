/**
  * @brief project TopNode, 实现了类Node的方法
  * @file node.cc
  * @author Liu Shuo
  */

#include "node.h"

/**
  * @class Node
  * @brief the consturct funciton of class Node
  * @param [in] ID: the ID of node
  * @param [in] port: 服务器的服务端口
  * @param [in] ip: 节点的ip
  */
Node::Node(string ID, string port, in_addr_t ip, int property)
{
    this->ID = ID;
    this->heartBeatTime = time(NULL);
    m_servicePort = atoi(port.c_str());
    //bzero(&m_socketAddress, sizeof(m_socketAddress));
    //m_socketAddress.sin_family = AF_INET;
    //m_socketAddress.sin_addr.s_addr = htons(INADDR_ANY);
    //m_socketAddress.sin_port = htons(m_servicePort);
    bzero(&m_serviceAddr, sizeof(m_serviceAddr));
    m_serviceAddr.sin_family = AF_INET;
    m_serviceAddr.sin_addr.s_addr = ip;
    m_serviceAddr.sin_port = htons(m_servicePort);
    m_sProMesg.property = property;
    Pthread_mutex_init(&statusLock, NULL);
    Pthread_mutex_init(&heatBeatTimeLock, NULL);
}

/**
  * @class Node
  * @brief 析构函数
  */
Node::~Node()
{
    pthread_mutex_destroy(&statusLock);
    pthread_mutex_destroy(&heatBeatTimeLock);
}

/**
  * @class Node
  * @brief 返回节点ID
  * @return 节点ID
  */
string Node::getID()
{
    return ID;
}

/**
  * @class Node
  * @brief 设置节点状态
  * @param [in] status 节点状态
  */
void Node::setstatus(int status)
{
    Pthread_mutex_lock(&statusLock);
    this->status = status;
    Pthread_mutex_unlock(&statusLock);
}

/**
  * @class Node
  * @brief 获得节点状态
  * @return 节点状态
  */
int Node::getstatus()
{
    Pthread_mutex_lock(&statusLock);
    int result = status;
    Pthread_mutex_unlock(&statusLock);
    return result;
}

/**
  * @class Node
  * @brief 设置节点上次心跳时间
  * @param [in] 心跳时间
  */
void Node::setheartBeatTime(time_t t)
{
    Pthread_mutex_lock(&heatBeatTimeLock);
    this->heartBeatTime = t;
    Pthread_mutex_unlock(&heatBeatTimeLock);
}

/**
  * @class Node
  * @brief 获得节点上次心跳时间
  * @return 心跳时间
  */
time_t Node::getheartBeatTime()
{
    Pthread_mutex_lock(&heatBeatTimeLock);
    time_t result = heartBeatTime;
    Pthread_mutex_unlock(&heatBeatTimeLock);
    return result;
}

/**
  * @class Node
  * @brief 显示节点详细信息
  * 该方法只用于调试
  */
void Node::display()
{
    cout << "node ID: " << ID << endl;
    cout << "node status: " << status << endl;
    cout << "node heart beat time: " << heartBeatTime << endl;
    cout << "node service port: " << m_servicePort << endl;
}

/**
  * @class Node
  * @brief set m_SocketAddress
  * @param [in] socketAddress: set m_SocketAddress socketAddress
  */
void Node::setSocketAddress(struct sockaddr_in socketAddress)
{
   // m_SocketAddress.sin_family = AF_INET;
   // m_SocketAddress.sin_addr.s_addr = htons(INADDR_ANY);
    //m_SocketAddress.sin_port = htons(servicePort);
}

/**
  * @class Node
  * @brief get m_socketAddress
  * @return m_socketAddress
  */
struct sockaddr_in Node::getSocketAddress()
{
    return m_socketAddress;
}

/**
  * @class Node
  * @brief 连接服务器
  * @return 套接字
  */
int Node::connectServer()
{
    int sockFd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(sockFd, (struct sockaddr *)&m_socketAddress, sizeof(m_socketAddress));
    return sockFd;
}

/**
  * @class Node
  * @brief 关闭服务器
  * @param [in] sockFd: 套接字
  */
void Node::closeServer(int sockFd)
{
    Close(sockFd);
}

/**
  * @class Node
  * @brief 向节点发送消息
  * @param [in] sockFd:
  * @param [in] buf:
  * @param [in] len:
  */
void Node::sendtoServer(int sockFd, char *buf, size_t len)
{
    Writen(sockFd, (void *)buf, len);
}

/**
  * @class Node
  * @brief 向节点发送UDP消息
  * @param [in] sockFd:
  * @param [in] ptr:
  * @param [in] len:
  */
void Node::sendtoRS(int sockfd, const void *ptr, size_t len)
{
    Sendto(sockfd, ptr, len, 0, (struct sockaddr *)&m_serviceAddr, sizeof(m_serviceAddr));
}

void Node::connecttoDB()
{
    m_sockdbfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dbaddr = m_serviceAddr;
    dbaddr.sin_port = htons(m_servicePort + 1);
    cout << m_servicePort + 1 << endl;
    Connect(m_sockdbfd, (struct sockaddr *)&dbaddr, sizeof(dbaddr));
    cout << m_sockdbfd << endl;
}
