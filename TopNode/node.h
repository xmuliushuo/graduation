/**
  * @brief project TopNode, 定义了类Node
  * @file node.h
  * @author Liu Shuo
  * @date 2011.4.18
  */

#ifndef NODE
#define NODE
#include "main.h"

/** @brief store the information of priority */
typedef struct _propertyMesg {
    int property;	/**< priority */
    int min;		/**<  */
    int max;		/**<  */
}s_propertyMesg;

/**
  * @brief 定义了类Node
  * @class Node
  * @author Liu Shuo
  */
class Node
{
public:
    Node(string, string, in_addr_t, int);	/**< constructor */
    ~Node();								/**< destructor */
    string getID();							/**< get the node ID */
    void setstatus(int);					/**< set node status LIVE: 节点正常运行, DEAD: 节点故障 */
    int getstatus();						/**< get node status */
    void setheartBeatTime(time_t);			/**< set the lastest heartbeat time */
    time_t getheartBeatTime();				/**< get the lastest heartbeat time */
    void display();							/**< display node information */
    void setSocketAddress(struct sockaddr_in);/**< being useless */
    struct sockaddr_in getSocketAddress();	/**< being useless */
    int connectServer();					/**< being useless */
    void closeServer(int);					/**< being useless */
    void sendtoServer(int, char *, size_t);	/**< being useless */
    void sendtoRS(int, const void *, size_t);/**< send a message to the server */
    void connecttoDB();

public:
    s_propertyMesg m_sProMesg;/**< store node's priority information */
    int m_sockdbfd;

private:
    string ID;		/**< 节点ID */
    int status;		/**< 节点状态 */
    int m_servicePort;  		/**< 节点接收请求的端口 */
    time_t heartBeatTime;		/**< 节点上次发出心跳的时间 */
    struct sockaddr_in m_socketAddress;	/**< 节点接收请求的地址结构 */
    struct sockaddr_in m_serviceAddr;	/**< 节点接收请求的地址结构 */
    pthread_mutex_t statusLock;			/**< 节点状态的互斥锁 */
    pthread_mutex_t heatBeatTimeLock;	/**< 节点心跳时间的互斥锁 */

};
#endif
