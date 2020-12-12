#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

using namespace std;

connection_pool::connection_pool()
{
	m_CurConn = 0;
	m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance()
{
	// 注意这里构造函数是私有的，只能类自己调用。且一个类只有一个。
	// 这里的单例模式实现的很巧妙啊。不是很能理解。
	// 这好像是Effective C++中提到的一种方法。
	// local static对象，只有在第一调用函数的时候再回初始化，之后就不会了。
	static connection_pool connPool;
	return &connPool;
}


//构造初始化,初始化最大连接数的mysql连接
void connection_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log)
{
	m_url = url;
	m_Port = Port;
	m_User = User;
	m_PassWord = PassWord;
	m_DatabaseName = DBName;
	m_close_log = close_log;

	for (int i = 0; i < MaxConn; i++)
	{
		MYSQL *con = NULL;
		con = mysql_init(con);

		if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}
		connList.push_back(con);
		++m_FreeConn;
	}

	// 这里是干什么呢，初始化信号量。用于给sql连接池加锁
	reserve = sem(m_FreeConn);

	m_MaxConn = m_FreeConn;
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *connection_pool::GetConnection()
{
	MYSQL *con = NULL;

	if (0 == connList.size())
		return NULL;
	// 申请资源
	reserve.wait();
	lock.lock();

	// 从list种获取一个连接资源
	con = connList.front();
	connList.pop_front();

	// 修改共享变量的值
	--m_FreeConn;
	++m_CurConn;

	// 解锁对资源的访问，允许其他线程访问这段代码。
	lock.unlock();
	return con;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MYSQL *con)
{
	if (NULL == con)  // 注意看这里写的非常的讲究
		return false;

	lock.lock();

	connList.push_back(con);
	++m_FreeConn;
	--m_CurConn;

	// lock用于进程间同步
	lock.unlock();
	// 修改信号量
	reserve.post();
	return true;
}

//销毁数据库连接池
void connection_pool::DestroyPool()
{

	lock.lock();
	if (connList.size() > 0)
	{
		list<MYSQL *>::iterator it;
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL *con = *it;
			mysql_close(con);
		}
		m_CurConn = 0;
		m_FreeConn = 0;
		connList.clear();
	}

	lock.unlock();
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
	return this->m_FreeConn;
}

connection_pool::~connection_pool()
{
	DestroyPool();
}



connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool){
	*SQL = connPool->GetConnection();
	
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}