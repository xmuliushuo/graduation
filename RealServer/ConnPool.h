/**
  * @brief project TopNode, 定义了ConnPool
  * @file ConnPool.h
  * @author Liu Shuo
  * @date 2011.5.4
  */

#ifndef CONNPOOL
#define CONNPOOL

#include "main.h"

/** @brief 存放每个连接的地址和状态*/
typedef struct _sConStatus
{
    MYSQL *connAddr;/**< 数据库连接地址 */
    int status;     /**< 数据库连接状态 0：空闲 1：使用中*/
}sConStatus;

/**
  * @brief 定义了类ConnPool
  * @class ConnPool
  * @author Liu Shuo
  */
class ConnPool
{
public:
    ConnPool(); /**< */
    ~ConnPool();/**< */
    int init(char *, char *, char *, char *, int, int);/**< connection  pool init */
    MYSQL *getOneConn();     /**< 从连接池取一个连接 */
    void retOneConn(MYSQL *);/**< 连接用完了，把它放回连接池。以便其他人用*/
    MYSQL *createOneConn();/**< 创建一个数据库连接 */

private:
    string m_strHost;   /**< 数据库地址 */
    string m_strUser;   /**< 数据库用户名 */
    string m_strPwd;    /**< 密码 */
    string m_strDbName; /**< 数据库名 */
    int m_intMysqlPort; /**< 数据库服务器端口 */
    int m_intConnNum;   /**< 最大连接数 */
    vector<sConStatus *> m_vectorConn;          /**< 存放连接的容器 */
    map<sConStatus *, int> m_mapVI;             /**< 从连接的地址，快速找到索引 */
    map<MYSQL *, sConStatus *> m_mapMysqlScs;   /**< 从连接快速找到状态 */
    pthread_mutex_t m_mutexScs;                 /**< 互斥锁 */
};
#endif
