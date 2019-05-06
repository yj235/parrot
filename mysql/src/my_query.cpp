#include <mysql.h>

#include <stdio.h>

bool my_query(const char *p){
	MYSQL *conn = mysql_init(NULL);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	mysql_real_connect(conn, "localhost", "yj", "a", "yj_db", 0, NULL, 0);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	if(mysql_query(conn, p)){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	MYSQL_RES *result = mysql_store_result(conn);
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row = mysql_fetch_row(result);

	mysql_free_result(result);
	mysql_close(conn);

	return row ? true : false;
}
