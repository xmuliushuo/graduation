/**
  * @brief project TopNode,
  * @file ConnPool.cc
  * @author Liu Shuo
  * @date 2011.4.18
  */

#include "ConnPool.h"
#include "main.h"

/**
  * @class ConnPool
  * @brief 构造函数
  */
ConnPool::ConnPool()
{
    Pthread_mutex_init(&m_mutexScs, NULL);
}

/**
  * @class ConnPool
  * @brief 析构函数
  */
ConnPool::~ConnPool()
{
}

/**
  * @class ConnPool
  * @brief 创建一个连接，并设为空闲状态。
  * @return 创建好的连接
  */
MYSQL *ConnPool::createOneConn()
{
    MYSQL *mysql;
    mysql = mysql_init(0);
    if (mysql == NULL) {
        cout << "mysql init error" << endl;
        exit(1);
    }
	if (mysql_real_connect(mysql, m_strHost.c_str(), m_strUser.c_str(), m_strPwd.c_str(), m_strDbName.c_str(), m_intMysqlPort, NULL, 0) == NULL) {
        cout << "mysql connect error" << endl;
        exit(1);
	}
	//cout << "create one connection successfully" << endl;
    return mysql;
}

/**
  * @class ConnPool
  * @brief 初始化连接池
  * @return 成功： 返回0
  */
int ConnPool::init(char *strHost, char *strUser, char *strPwd, char *strDbName, int intMysqlPort, int intConnNum)
{
    cout << "初始化数据库连接池" << endl;
    m_strHost.assign(strHost);
    m_strUser.assign(strUser);
    m_strPwd.assign(strPwd);
    m_strDbName.assign(strDbName);
    m_intMysqlPort = intMysqlPort;
    m_intConnNum = intConnNum;

    MYSQL *mysql;
    for (int i = 0; i < intConnNum; i++) {
        mysql = createOneConn();
        sConStatus *scs = new sConStatus();
        scs->connAddr = mysql;
        scs->status = 0;
        m_vectorConn.push_back(scs);
        m_mapVI[scs] = i;
        m_mapMysqlScs[mysql] = scs;
    }
    cout << "初始化数据库连接池完毕" << endl;
    return  0;
}

/**
  * @class ConnPool
  * @brief 取得一个连接
  * @return 成功：返回连接 失败：返回NULL
  */
MYSQL *ConnPool::getOneConn()
{
    Pthread_mutex_lock(&m_mutexScs);
    for (int i = 0; i < m_intConnNum; i++) {
        sConStatus *scs = m_vectorConn[i];
        if (scs->status == 0) {
            scs->status = 1;
            Pthread_mutex_unlock(&m_mutexScs);
            return scs->connAddr;
        }
    }
    Pthread_mutex_unlock(&m_mutexScs);
    return NULL;
}

/**
  * @class ConnPool
  * @brief 释放一个连接
  * @param [in] pMysql 要释放的连接
  */
void ConnPool::retOneConn(MYSQL *pMysql)
{
    if (!pMysql)
        return;
    map<MYSQL *, sConStatus *>::iterator it1;
    map<sConStatus *, int>::iterator it2;

    it1 = m_mapMysqlScs.find(pMysql);
    if (it1 == m_mapMysqlScs.end())
        return;
    it2 = m_mapVI.find(it1->second);
    if (it2 == m_mapVI.end())
        return;
    int index = it2->second;
    Pthread_mutex_lock(&m_mutexScs);
    m_vectorConn[index]->status = 0;
    Pthread_mutex_unlock(&m_mutexScs);
}

