#include <stdlib.h>
#include <stdio.h>
#include "mysql/mysql.h"

int main( int argc, char *argv[] )
{
	MYSQL *conn_ptr;
	conn_ptr = mysql_init( NULL );	/* 连接初始化 */
	if ( !conn_ptr )
	{
		fprintf( stderr, "mysql_init failed\n" );
		return(EXIT_FAILURE);
	}

	conn_ptr = mysql_real_connect( conn_ptr, "0.0.0.0", "root", "Twn1949101", "webdb", 0, NULL, 0 );	/* 建立实际连接 */
	/* 参数分别为：初始化的连接句柄指针，主机名（或者IP），用户名，密码，数据库名，0，NULL，0）后面三个参数在默认安装mysql>的情况下不用改 */
	if ( conn_ptr ){
		printf( "Connection success\n" );
	}
	else{
		printf( "Connection failed\n" );
	}

	mysql_close( conn_ptr ); /* 关闭连接 */

	return(EXIT_SUCCESS);
}