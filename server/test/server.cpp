#include "KVP.h"
#include "format.h"
#include "user.h"
#include "pdebug.h"
#include "my_query.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <queue>
#include <deque>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXSIZE 1024

using namespace std;

unordered_map<int, sockaddr_in> socket_sockaddr_in;

//<用户名 端口> 用id? //有bug 登录
//unordered_map<string, int>name_socket;
//<端口 用户名> 用id? //有bug 登录
//unordered_map<int, string>socket_name;
//<id 密码>
unordered_map<unsigned int, string>id_password;
//<端口,id>
unordered_map<int, unsigned int>socket_id;
//<id,端口>
//多设备用map<id,unordered_set<socket>>?
unordered_map<unsigned int, int>id_socket;

//联系人消息队列
//super simple version
unordered_map<unsigned int, queue<string>> contacts_mq;
//id-联系人-消息队列
unordered_map<unsigned int, unordered_map<unsigned int, queue<string>>> id_contacts_mq;

//群号 成员map
unordered_map<unsigned int, unordered_set<unsigned int>> groupID_memberID;
//群消息队列 group_id
unordered_map<unsigned int, deque<string>> group_mq;
//用户未读群消息数
unordered_map<unsigned int, unordered_map<unsigned int, unsigned int>> userID_groupID_num;

//用户集合
//unordered_set<User> users;

//联系人 群 对话窗口状态
//默认初始化为false吗?
typedef struct contacts_group_status{
	unordered_map<unsigned int, bool> contacts_status;
	unordered_map<unsigned int, bool> group_status;
}contacts_group_status;

//<id contacts_groups_status>
unordered_map<unsigned int, contacts_group_status> id_cgs;

//互加联系人
//***事务回滚***没做
int add_contacts_to_list(int client_socket, string target_id_str){
	unsigned int target_id = stoi(target_id_str);
	unsigned int source_id = socket_id[client_socket];
	if (source_id == target_id) {
		return -1;
	}
	string source_id_str = to_string(source_id);
	string sql1 = "INSERT INTO " + source_id_str + "_contacts(contacts_id) SELECT " + target_id_str + " FROM dual WHERE NOT EXISTS(SELECT contacts_id FROM " + source_id_str + "_contacts WHERE contacts_id=" + target_id_str + ")";
	if (my_query_int(sql1)) {
		pdebug << "insert into" + to_string(source_id) + "_contacts failed";
		return -1;
	}
	string sql2 = "INSERT INTO " + target_id_str + "_contacts(contacts_id) SELECT " + source_id_str + " FROM dual WHERE NOT EXISTS(SELECT contacts_id FROM " + target_id_str + "_contacts WHERE contacts_id=" + source_id_str + ")";
	if (my_query_int(sql2)) {
		pdebug << "insert into " + to_string(target_id) + "_contacts failed";
		return -1;
	}
	return 0;
}

int add_group_to_list(int client_socket, unsigned int group_id){
	unsigned int user_id = socket_id[client_socket];
	string user_id_str = to_string(user_id);
	string group_id_str = to_string(group_id);
	string sql1 = "INSERT INTO group_" + group_id_str + "(user_id) SELECT " + user_id_str + " FROM dual WHERE NOT EXISTS(SELECT user_id FROM group_" + group_id_str + " WHERE user_id=" + user_id_str + ")";
	if (my_query_int(sql1)) {
		pdebug << "insert into group_" + to_string(group_id) + "failed";
		return -1;
	}
	string sql2 = "INSERT INTO " + user_id_str + "_group(group_id) SELECT " + group_id_str + " FROM dual WHERE NOT EXISTS(SELECT group_id FROM " + user_id_str + "_group WHERE group_id=" + group_id_str + ")";
	if (my_query_int(sql2)) {
		pdebug << "insert into " + to_string(group_id) + "_group failed";
		return -1;
	}
	groupID_memberID[group_id].insert(user_id);
	return 0;
}

void my_send(int socket, string &str){
	//most stupid way to do this..
	char data[1024];
	memset(data, 0, sizeof(data));
	unsigned int len = str.length();
	memcpy(data, &len, sizeof(int));
	memcpy(data + sizeof(len), str.c_str(), str.length());
	send(socket, data, sizeof(len) + len, 0);
}

void query_name(int client_socket, const rapidjson::Value &object) {
	//void query_name(int client_socket, const rapidjson::Document &doc) {
	string name = object["name"].GetString();
	//string name = doc["query name"].GetString();
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("query");
	writer.StartObject();
	writer.Key("name");
	string sql = "select id, password from user where name=\"" + name + "\"";
	vector<vector<string>> vvs(my_query(sql));
	if (vvs.empty()) {
		//pdebug << "name not exist" << endl;
		//writer.String("not exist");
		writer.Null();
	} else {
		//pdebug << "name exist" << endl;
		unsigned int id = atoi(vvs[0][0].c_str());
		socket_id[client_socket] = id;
		id_socket[id] = client_socket;
		string password = vvs[0][1];
		id_password[id] = password;
		//writer.String("exist");
		writer.Uint(id);

		//User user(id, name, password);
		//users.insert(user);
	}
	writer.EndObject();
	writer.EndObject();
	string data_send = sb.GetString();
	my_send(client_socket, data_send);
}

void query_password(int client_socket, const rapidjson::Value &object){
	string password = object["password"].GetString();
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("query");
	writer.StartObject();
	writer.Key("password");
	if (id_password[socket_id[client_socket]] == password) {
		//name_socket[socket_name[client_socket]] = client_socket;
		//pdebug << "password correct" << endl;
		writer.String("correct");
	} else {
		//pdebug << "password incorrect" << endl;
		writer.String("incorrect");
	}
	writer.EndObject();
	writer.EndObject();
	string data_send = sb.GetString();
	my_send(client_socket, data_send);
}

void query_contacts(int client_socket) {
	string id = to_string(socket_id[client_socket]);
	string contacts_query = "select user.id, user.name from user, " + id + "_contacts where user.id=" + id + "_contacts.contacts_id";
	vector<vector<string>> vvs(my_query(contacts_query));
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("contacts list");
	writer.StartArray();
	for (auto &v : vvs) {
		writer.StartObject();
		writer.Key("id");
		writer.Uint(stoul(v[0]));
		writer.Key("name");
		writer.String(v[1].c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	string data(sb.GetString());
	//pdebug << data << endl;
	my_send(client_socket, data);
}

void query_group(int client_socket){
	string id = to_string(socket_id[client_socket]);
	string sql = "select groups.id, groups.name from groups, " + id + "_group where groups.id=" + id + "_group.group_id";
	vector<vector<string>> vvs(my_query(sql));
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("group list");
	writer.StartArray();
	for (auto &v : vvs) {
		writer.StartObject();
		writer.Key("id");
		writer.Uint(stoul(v[0]));
		writer.Key("name");
		writer.String(v[1].c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	string data(sb.GetString());
	//pdebug << data << endl;
	my_send(client_socket, data);
}

void regist(int client_socket, const rapidjson::Value &object){
	string name, password;
	if (object.HasMember("name") && object["name"].IsString()) {
		name = object["name"].GetString();
	}
	if (object.HasMember("password") && object["password"].IsString()) {
		password = object["password"].GetString();
	}
	string sql = "insert into user(name, password) values(\"" + name + "\", \"" + password + "\")";
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("regist");
	if (my_query_int(sql)) {
		writer.String("failed");
		pdebug << "insert user failed" << endl;
	} else {
		writer.String("success");
		string query_id = "select id from user where name=\"" + name + "\"";
		socket_id[client_socket] = stoul(my_query(query_id)[0][0]);
		id_socket[socket_id[client_socket]] = client_socket;
		string create_contacts = "create table " + to_string(socket_id[client_socket]) + "_contacts(id int unsigned not null auto_increment primary key, contacts_id int unsigned not null)";
		if (my_query_int(create_contacts)) {
			pdebug << "create contacts failed" << endl;
		}
		string create_group = "create table " + to_string(socket_id[client_socket]) + "_group(id int unsigned not null auto_increment primary key, group_id int unsigned not null)";

		if (my_query_int(create_group)) {
			pdebug << "create " + to_string(socket_id[client_socket]) + "_group failed";
		}
	}
	writer.EndObject();
	string data = sb.GetString();
	my_send(client_socket, data);
}

void send(int client_socket, const rapidjson::Value &object){
	unsigned target_id = object["id"].GetUint();
	string time(object["time"].GetString());
	string message_received(object["message"].GetString());
	//可以修改json 还不会
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("send");
	writer.StartObject();
	writer.Key("id");
	unsigned int source_id = socket_id[client_socket];
	writer.Uint(source_id);
	writer.Key("time");
	writer.String(time.c_str());
	writer.Key("message");
	writer.String(message_received.c_str());
	writer.EndObject();
	writer.EndObject();
	string data(sb.GetString());
	pdebug << data << endl;
	//v1 在线发送 离线入列
	//v2 改为客户端打开联系人窗口发送 否则入列 ing...
	//做一个开关?
	if (id_socket.count(target_id) && id_cgs[target_id].contacts_status[source_id]) {
		pdebug << "to send" << endl;
		my_send(id_socket[target_id], data);
	} else {
		pdebug << "to queue" << endl;
		//contacts_mq[target_id].push(data_send);
		id_contacts_mq[target_id][source_id].push(data);
	}
}

void send_group(int client_socket, const rapidjson::Value &object, string &data){
	unsigned int group_id = object["group id"].GetUint();
	unsigned int user_id = object["user id"].GetUint();
	group_mq[group_id].push_back(data);
	for (auto &v : groupID_memberID[group_id]) {
		if (id_socket.find(v) != id_socket.end() && id_cgs[v].group_status[group_id]) {
			//pdebug << "send" << endl;
			my_send(id_socket[v], data);
		} else {
			//pdebug << "++" << endl;
			++userID_groupID_num[v][group_id];
		}
	}
}

void search_contacts(int client_socket, const rapidjson::Document &doc){
	string id = doc["id"].GetString();
	string sql = "select name from user where id=" + id;
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("search");
	writer.String("contacts");
	writer.Key("name");
	vector<vector<string>> vvs(my_query(sql));
	if (vvs.empty()) {
		writer.String("not exist");
	} else {
		writer.String(vvs[0][0].c_str());
		add_contacts_to_list(client_socket, id);
	}
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);
}

void new_member(unsigned int group_id, unsigned int id){
	string query_name = "select name from user where id=" + to_string(id);
	vector<vector<string>> vvs(my_query(query_name));
	string name(vvs[0][0]);
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("new member");
	writer.StartObject();
	writer.Key("group id");
	writer.Uint(group_id);
	writer.Key("id");
	writer.Uint(id);
	writer.Key("name");
	writer.String(name.c_str());
	writer.EndObject();
	writer.EndObject();
	string data(sb.GetString());
	for (auto &v : groupID_memberID[group_id]) {
		if (id_socket.find(v) != id_socket.end() && id_cgs[v].group_status[group_id]) {
			my_send(id_socket[v], data);
		}
	}
}

void search_group(int client_socket, const rapidjson::Document &doc){
	unsigned int id = doc["id"].GetUint();
	unsigned int group_id = doc["group id"].GetUint();
	string sql = "select name from groups where id=" + to_string(group_id);
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("search");
	writer.String("group");
	writer.Key("name");
	vector<vector<string>> vvs(my_query(sql));
	if (vvs.empty()) {
		writer.String("not exist");
	} else {
		writer.String(vvs[0][0].c_str());
		add_group_to_list(client_socket, group_id);
	}
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);

	//发送添加新成员消息
	new_member(group_id, id);
}

void query_groupMember(int client_socket, rapidjson::Document &doc){
	unsigned int id = doc["id"].GetUint();
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("member list");
	writer.StartObject();
	writer.Key("group id");
	writer.Uint(id);
	writer.Key("list");
	writer.StartArray();
	string sql = "select user.id, user.name from user, group_" + to_string(id) + " where user.id=group_" + to_string(id) + ".user_id";
	vector<vector<string>> vvs(my_query(sql));
	for (auto &v : vvs) {
		writer.StartObject();
		writer.Key("id");
		writer.Uint(atol(v[0].c_str()));
		writer.Key("name");
		writer.String(v[1].c_str());
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	writer.EndObject();
	string data(sb.GetString());
	//pdebug << data << endl;
	my_send(client_socket, data);
}

void query_id(int client_socket){
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("query id");
	//writer.String("id");
	//writer.Key("id");
	writer.Uint(socket_id[client_socket]);
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);
}

void contacts_window_open(int client_socket, const rapidjson::Value &object) {
	unsigned int id = object["id"].GetUint();
	unsigned int contacts_id = object["contacts_id"].GetUint();
	id_cgs[id].contacts_status[contacts_id] = true;
	while (!id_contacts_mq[id][contacts_id].empty()) {
		string data(id_contacts_mq[id][contacts_id].front());
		my_send(client_socket, data);
		id_contacts_mq[id][contacts_id].pop();
	}
}

void contacts_window_close(int client_socket, const rapidjson::Value &object){
	unsigned int id = object["id"].GetUint();
	unsigned int contacts_id = object["contacts_id"].GetUint();
	id_cgs[id].contacts_status[contacts_id] = false;
}

void group_window_open(int client_socket, const rapidjson::Value &object) {
	unsigned int id = object["id"].GetUint();
	unsigned int group_id = object["group_id"].GetUint();
	id_cgs[id].group_status[group_id] = true;
	auto end = group_mq[group_id].end();
	pdebug << userID_groupID_num[id][group_id] << endl;
	auto index = end - userID_groupID_num[id][group_id];
	userID_groupID_num[id][group_id] = 0;
	for (; index != end; ++index) {
		my_send(client_socket, *index);
	}
}

void group_window_close(int client_socket, const rapidjson::Value &object){
	unsigned int id = object["id"].GetUint();
	unsigned int group_id = object["group_id"].GetUint();
	id_cgs[id].group_status[group_id] = false;
}

int create_group(int client_socket, const rapidjson::Value &object){
	unsigned int id = object["id"].GetUint();
	string name(object["name"].GetString());
	string insert_groups = "INSERT INTO groups(name) VALUES(\"" + name + "\")";
	if (my_query_int(insert_groups)) {
		pdebug << "insert into groups failed" << endl;
		return -1;
	}
	string query_id = "select id from groups where name=\"" + name + "\"";
	vector<vector<string>> vvs(my_query(query_id));
	unsigned int group_id = atoi(vvs[0][0].c_str());
	string create_group = "CREATE TABLE group_" + vvs[0][0] + " (id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, user_id INT UNSIGNED NOT NULL)";
	if (my_query_int(create_group)) {
		pdebug << "create group_id failed" << endl;
	}
	string insert_user = "INSERT INTO group_" + vvs[0][0] + "(user_id) VALUES(" + to_string(id) + ")";
	my_query_int(insert_user);
	string insert_group = "INSERT INTO " + to_string(id) + "_group(group_id) VALUES(" + vvs[0][0] + ")";
	my_query_int(insert_group);
	groupID_memberID[group_id].insert(id);
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("create group");
	writer.StartObject();
	writer.Key("group id");
	writer.Uint(group_id);
	writer.Key("group name");
	writer.String(name.c_str());
	writer.EndObject();
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);
}

void parse(int client_socket, string &data){
	rapidjson::Document doc;
	if (doc.Parse(data.data()).HasParseError()) {
		cerr << "json parse error" << endl;
		return;
	}
	if (doc.HasMember("query name")) {
		const rapidjson::Value &object = doc["query name"];
		query_name(client_socket, object);

	} else if (doc.HasMember("create group")) {
		const rapidjson::Value &object = doc["create group"];
		create_group(client_socket, object);

	} else if (doc.HasMember("contacts window open")) {
		const rapidjson::Value &object = doc["contacts window open"];
		contacts_window_open(client_socket, object);
	} else if (doc.HasMember("contacts window close")) {
		const rapidjson::Value &object = doc["contacts window close"];
		contacts_window_close(client_socket, object);

	} else if (doc.HasMember("group window open")) {
		const rapidjson::Value &object = doc["group window open"];
		group_window_open(client_socket, object);
	} else if (doc.HasMember("group window close")) {
		const rapidjson::Value &object = doc["group window close"];
		group_window_close(client_socket, object);

	} else if (doc.HasMember("query password")) {
		const rapidjson::Value &object = doc["query password"];
		query_password(client_socket, object);
	} else if (doc.HasMember("query") && doc["query"].IsString() && doc["query"].GetString() == string("contacts")) {
		query_contacts(client_socket);
	} else if (doc.HasMember("query") && doc["query"].IsString() && doc["query"].GetString() == string("group")) {
		query_group(client_socket);
	} else if (doc.HasMember("regist") && doc["regist"].IsObject()) {
		const rapidjson::Value &object = doc["regist"];
		regist(client_socket, object);
	} else if (doc.HasMember("send") && doc["send"].IsObject()) {
		const rapidjson::Value &object = doc["send"];
		send(client_socket, object);
	} else if (doc.HasMember("search") && doc["search"].IsString() && doc["search"].GetString() == string("contacts")) {
		search_contacts(client_socket, doc);
	} else if (doc.HasMember("search") && doc["search"].IsString() && doc["search"].GetString() == string("group")) {
		search_group(client_socket, doc);
	} else if (doc.HasMember("query") && doc["query"].IsString() && doc["query"].GetString() == string("group member")) {
		query_groupMember(client_socket, doc);
	} else if (doc.HasMember("send group") && doc["send group"].IsObject()) {
		rapidjson::Value &object = doc["send group"];
		send_group(client_socket, object, data);
	} else if (doc["query"].GetString() == string("id")) {
		query_id(client_socket);
	//} else if (doc.HasMember("create group")) {
	//	const rapidjson::Value &object = doc["create group"];
	//	create_group(client_socket, object);
	} else {
		pdebug << "other" << endl;
	}
}

void *client_thread(void *_client_socket){
	char data[MAXSIZE];
	int client_socket = *(int*)_client_socket;
	//struct in_addr in = socket_socketaddr_in[client_sock].sin_addr.s_addr;
	pdebug << socket_sockaddr_in[client_socket].sin_addr.s_addr;
	pdebug << inet_ntoa(socket_sockaddr_in[client_socket].sin_addr) << endl;
	unsigned int len = 0;
	int ret = 0;
	while(1){
		ret = recv(client_socket, &len, sizeof(len), 0);
		memset(data, 0, sizeof(data));
		int id = socket_id[client_socket];
		if (0 == ret) {
			//断开链接
			if (0 == socket_id.count(client_socket)){
				break;
			}
			id_socket.erase(id);
			socket_id.erase(client_socket);
			break;
		} else if (-1 == ret) {
			id_socket.erase(id);
			socket_id.erase(client_socket);
			pdebug << errno << endl;
			break;
		}
		ret = recv(client_socket, data, len, 0);
		//pdebug << "recv " << ret << endl;
		string s(data);
		pdebug << data << endl;
		parse(client_socket, s);
	}
}

//初始化
//群
void init(void){
	string sql_group_id = "select id from groups";
	vector<vector<string>> vvs(my_query(sql_group_id));
	for (auto &v : vvs) {
		unsigned int group_id = atol(v[0].c_str());
		string sql_user_id = "select user_id from group_" + v[0];
		vector<vector<string>> vvs2(my_query(sql_user_id));
		for (auto &v2 : vvs2) {
			unsigned int user_id = atol(v2[0].c_str());
			groupID_memberID[group_id].insert(user_id);
		}
	}
}

//要改
void* server_thread(void* _client_socket){
	/******/
}

int main(){
	init();
	unsigned short port = 8080;
	struct sockaddr_in server_sockaddr;

	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//设置调用closesocket()后，仍可继续重用该socket.(调用closesocket()一般不会立即关闭socket,而经历TIME_WAIT的过程.) //好像并没有用?
	bool bReuseaddr = true;
	setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(bReuseaddr));

	memset(&server_sockaddr, 0, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(port);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(server_sockfd, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr));
	//!!!
	listen(server_sockfd, 2);

	//线程分离
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_t server_thread_t;
	pthread_create(&server_thread_t, &attr, server_thread, NULL);

	while(true){
		struct sockaddr_in client_socketaddr;
		socklen_t client_sockaddr_len = sizeof(client_socketaddr);

		int client_socket = accept(server_sockfd, (struct sockaddr*)&client_socketaddr, &client_sockaddr_len);
		//端口 地址信息
		socket_sockaddr_in[client_socket] = client_socketaddr;

		pthread_t tid;
		pthread_create(&tid, &attr, client_thread, &client_socket);
	}

	while(1);

	close(server_sockfd);
}
