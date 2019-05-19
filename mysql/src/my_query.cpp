#include "../include/test/my_query.h"
//#include "~/a/parrot/mysql/include/test/my_query.h"

#include <mysql.h>

char* my_query(const char *p){
	MYSQL *conn = mysql_init(NULL);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	mysql_real_connect(conn, "localhost", "yj", "a", "yj_db", 0, NULL, 0);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	if(mysql_query(conn, p)){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return NULL;
	}
	MYSQL_RES *result = mysql_store_result(conn);
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row = mysql_fetch_row(result);

	//释放 传参 问题
	mysql_free_result(result);
	mysql_close(conn);

	if(row){
		//返回值问题 局部变量
		return row[0];
	} 
	return NULL;
}
