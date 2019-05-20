#include "../../include/test/my_query.h"
//#include "~/a/parrot/mysql/include/test/my_query.h"

#include <mysql.h>

using namespace std;

//string my_query(const char *p){
vector<vector<string>> my_query(const string &s){
	vector<vector<string>> vvs;
	const char *p = s.c_str();
	MYSQL *conn = mysql_init(NULL);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		//return "";
	}
	mysql_real_connect(conn, "localhost", "yj", "a", "yj_db", 0, NULL, 0);
	if(conn == NULL){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		//return "";
	}
	if(mysql_query(conn, p)){
		fprintf(stderr, "%u %s\n", mysql_errno(conn), mysql_error(conn));
		//return "";
	}
	MYSQL_RES *result = mysql_store_result(conn);
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	while(row = mysql_fetch_row(result)){
		vector<string> vs;
		for(int i = 0; i < num_fields; ++i){
			vs.push_back(row[i] ? row[i] : "NULL");
		}
		vvs.push_back(vs);
	}

	//释放 传参 问题
	mysql_free_result(result);
	mysql_close(conn);

	return vvs;
}

char *my_query_2(const char *p){
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
		return row[0];
	} 
	return NULL;
}

int my_query_int(const string &sql){
	MYSQL *conn = mysql_init(NULL);
	mysql_real_connect(conn, "localhost", "yj", "a", "yj_db", 0, NULL, 0);
	int ret = -1;
	//const char *p = sql.c_str();
	ret = mysql_query(conn, sql.c_str());
	mysql_close(conn);
	return ret;
}
