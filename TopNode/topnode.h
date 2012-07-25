/**
  * @brief project TopNode, 定义了类TopNode
  * @file topnode.h
  * @author Liu Shuo
  * @date 2011.4.18
  */

#include "main.h"
#include "node.h"
#include "ConnPool.h"
#ifndef TOPNODE
#define TOPNODE

/**
  * @brief 定义了类TopNode
  * @class TopNode
  * @author Liu Shuo
  */
class TopNode{
public:
    TopNode();			/**< 构造函数 */
    ~TopNode();			/**< 析构函数 */
    void init();		/**< initialize the topnode */
    Node *getNextNode();/**< 轮询获得下个可用节点 */
	void initProperty();/**< 初始化优先级 */
	void readConf(char *);				/**< 读取配置文件 */
    void *serve(void *arg);				/**< 对外提供服务 */
    void *receiveheartbeat(void *arg);	/**< 接收心跳 */
    void *receiveMessage(void *arg);	/**< 接收消息 */
    void *changeStatus(void *arg);		/**< 检查各字节点状态 */
    void *handleRequest(int, const struct sockaddr_in *, string *);	/**< 对请求进行处理 */
    void *handleMesg(int, const struct sockaddr_in *);		/**< 对消息进行处理 */
    Node *getNextNodeByIP(uint32_t);/**< 通过IP获得下个可用节点 */
    Node *getNextNodeByReq(int);	/**< 通过请求内容获得下个可用节点*/
    Node *getNextNodeByLoad();		/**< 通过负载获得下个可用节点*/
    Node *getNextNodeByProperty();	/**< 通过优先级获得下个可用节点*/
    void *handledbUpdate(int);
    void *dbUpdate(void *arg);
    void console();
    void exitLB();

private:
    vector<Node *> nodeVector;				/**< 存放节点信息的容器 */
    vector<Node *>::size_type m_nextNode;   /**< 转发请求时，下一个节点的序号 */
    pthread_t heatbeatPthread;				/**< 接受心跳的线程ID */
    pthread_t messagePthread;				/**< 接受消息的线程ID */
    pthread_t statusPthread;				/**< 变更状态的线程ID */
    pthread_t servePthread;					/**< 提供服务的线程ID */
    pthread_t m_dbPthread;
    pthread_mutex_t m_nextNodeLock;         /**< m_nextNode的互斥锁 */
    pthread_mutex_t m_propertyLock;			/**< property的互斥锁 */
    pthread_mutex_t m_threadNumLock;        /**< threadNum的互斥锁 */
    pthread_mutex_t m_dbUpdateLock;
    int m_heartbeatPort;/**< 接收心跳的端口 */
    int m_messagePort;  /**< 接收消息的端口 */
    int m_dbPort;
    int m_servicePort;  /**< 对外提供服务的端口 */
    int m_policy;       /**< 客户端请求转发策略 0：不转发 1：采用轮询的方式转发 2: 根据源IP哈希转发 3：根据请求内容哈希转发 4：根据负载转发 */
    int m_sockfd;		/**< 提供服务的套接字 */
    int m_count;		/**< 接受请求计数 */
    int m_rsNum;		/**< 二层服务器的数量 */
    int m_propertySum;	/**< 优先级之和，用于优先级调度 */
    int m_threadNum;    /**< 线程数 */
    int m_maxThreadNum; /**< 最大线程数 */
    ConnPool *m_pool;   /**< 数据库连接池 */
    FILE *m_log;          /**< 日志文件 */
};

/** @brief the arg of the function thread_handleRequest */
struct requestArgs{
    TopNode *topNode;           /**< 指向TopNode */
    struct sockaddr_in cliaddr; /**< 客户端的地址结构*/
    int connfd;                 /**< 客户端的socket套接字*/
    string *request;             /**< 客户端的请求内容*/
};

#endif
