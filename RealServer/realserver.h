/**
  * @brief project RealServer, 定义了类RealServer
  * @file realserver.h
  * @author Liu Shuo
  * @date 2011.4.18
  */

#ifndef REALSERVER
#define REALSERVER
#include "main.h"
#include "ConnPool.h"

/**
  * @brief 定义了类RealServer
  * @class RealServer
  * @author Liu Shuo
  */
class RealServer{
public:
    RealServer();
    ~RealServer();
    //send heartbeat to the top node
    void *heartbeat(void *arg);
    //register to the top node
    //return 0: success
    //return -1: fail
    int registerToTopNode();
    //initialize the server
    void init();/**< 初始化服务器 */
    void *serviceTCP(void *arg);
    void *serviceUDP(void *arg);
    void *handleRequest(int, const struct sockaddr_in *, string *);
    void readConf(char *);                  /**< 读取配置文件 */
    void *dbUpdate(void *);
private:
    pthread_t heartbeatPthread;
    pthread_t servicePthread;
    char m_lbIP[20];                        /**< 负载均衡服务器的地址 */
    int m_lbHbPort;                         /**< 负载均衡服务器的接收心跳的端口 */
    int m_lbMessagePort;                    /**< 负载均衡服务器的接收消息的端口 */
    string m_ID;                            /**< 节点ID */
    int m_servicePort;                      /**< 提供服务的端口 */
    int m_dbPort;                           /**< 接收数据库更新的端口 */
    struct sockaddr_in serverMessageAddr;/**< the address of the LB which to receive message */
    int m_serviceMode;
    int m_property;
    ConnPool *m_pool;                       /**< 数据库连接池 */
    int m_connNum;                          /**< 处理请求数 */
    int m_dbNum;
    int m_sockfd;       /**< 提供服务的套接字 */
    int m_threadNum;    /**< 线程数 */
    int m_maxThreadNum; /**< 最大线程数 */
    pthread_mutex_t m_threadNumLock;        /**< threadNum的互斥锁 */
    FILE *m_log;    /**< 日志文件 */
};

struct requestArgs{
    RealServer *realServer;     /**< 指向RealServer */
    struct sockaddr_in cliaddr; /**< 客户端的地址结构*/
    int connfd;                 /**< 客户端的socket套接字*/
    string *request;            /**< 客户端的请求内容*/
};
#endif
