#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;

class connection_pool
{
public:
	MYSQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MYSQL *conn); //释放连接
	int GetFreeConn();					 //获取连接
	void DestroyPool();					 //销毁所有连接

	//单例模式 [唯一的创建一个MySQL连接池]
	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log); 

public:
	string m_url;			 //主机地址
	string m_Port;		 //数据库端口号
	string m_User;		 //登陆数据库用户名
	string m_PassWord;	 //登陆数据库密码
	string m_DatabaseName; //使用数据库名
	int m_close_log;	//日志开关

private:
	connection_pool();   // 由于是单例模式，所以构造函数和析构函数都属于private，只能自己调用
	~connection_pool();

	int m_MaxConn;  //最大连接数
	int m_CurConn;  //当前已使用的连接数
	int m_FreeConn; //当前空闲的连接数
	locker lock;
	list<MYSQL *> connList; //连接池
	sem reserve;
};


// 这个类的功能大概是用来管理这个mysql连接池的，感觉另一个目的是防止用户使用的时候，忘了释放mysql连接造成资源的浪费。
// 使用了这个类之后，只需要申请使用资源就好了，不用太关注资源的释放，当这个类被析构之后，资源就被释放了。】
// 这个设计很妙啊
class connectionRAII{

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	connection_pool *poolRAII;
};

#endif
