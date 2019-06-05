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

//<用户名 端口> 用id? //有bug 登录
unordered_map<string, int>name_socket;
//<端口 用户名> 用id? //有bug 登录
unordered_map<int, string>socket_name;
//<用户名 密码>
unordered_map<string, string>name_password;
//<端口,id>
unordered_map<int, unsigned int>socket_id;
//<id,端口>
//多设备用map<id,vector<socket>>?
unordered_map<unsigned int, int>id_socket;
//聊天室 <房间号,socket_fd>
unordered_map<string, unordered_set<int>> room;
//联系人消息队列
//super simple version
unordered_map<unsigned int, queue<string>> contacts_mq;
//群号 成员map
unordered_map<unsigned int, unordered_set<unsigned int>> groupID_memberID;
//群消息队列 group_id
unordered_map<unsigned int, deque<string>> group_mq;
//用户未读群消息数
unordered_map<unsigned int, unordered_map<unsigned int, unsigned int>> userID_groupID_num;

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
	string name = object["name"].GetString();
	socket_name[client_socket] = name;
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("query");
	writer.StartObject();
	writer.Key("name");
	string sql = "select id, password from user where name=\"" + name + "\"";
	vector<vector<string>> vvs;
	if ((vvs = my_query(sql)).empty()) {
		pdebug << "name not exist" << endl;
		writer.String("not exist");
	} else {
		pdebug << "name exist" << endl;
		socket_id[client_socket] = stoul(vvs[0][0]);
		id_socket[socket_id[client_socket]] = client_socket;
		string password = vvs[0][1];
		name_password[name] = password;
		writer.String("exist");
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
	if (name_password[(socket_name[client_socket])] == password) {
		name_socket[socket_name[client_socket]] = client_socket;
		pdebug << "password correct" << endl;
		writer.String("correct");
	} else {
		pdebug << "password incorrect" << endl;
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
	pdebug << data << endl;
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
	pdebug << data << endl;
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
	unsigned id = object["id"].GetUint();
	string time(object["time"].GetString());
	string message_received(object["message"].GetString());
	//可以修改json 还不会
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("send");
	writer.StartObject();
	writer.Key("id");
	writer.Uint(socket_id[client_socket]);
	writer.Key("time");
	writer.String(time.c_str());
	writer.Key("message");
	writer.String(message_received.c_str());
	writer.EndObject();
	writer.EndObject();
	string data_send(sb.GetString());
	pdebug << data_send << endl;
	//在线发送 离线入列
	//做一个开关?
	if (id_socket.count(id)) {
		pdebug << "to send" << endl;
		my_send(id_socket[id], data_send);
	} else {
		pdebug << "to queue" << endl;
		contacts_mq[id].push(data_send);
	}
}

void send_group(int client_socket, const rapidjson::Value &object, string &data){
	unsigned group_id = object["group id"].GetUint();
	unsigned int user_id = object["user id"].GetUint();
	//string time = object["time"].GetString();
	//string message = object["time"].GetString();
	group_mq[group_id].push_back(data);
	for (auto &v : groupID_memberID[group_id]) {
		//若不在线 用户->群->消息计数+1;
		if (id_socket.find(v) == id_socket.end()) {
			++userID_groupID_num[user_id][group_id];
		} else {
			//若在线 直接发送
			my_send(id_socket[v], data);
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

void search_group(int client_socket, const rapidjson::Document &doc){
	unsigned int id = doc["id"].GetUint();
	string sql = "select name from groups where id=" + to_string(id);
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
		add_group_to_list(client_socket, id);
	}
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);
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
	pdebug << data << endl;
	my_send(client_socket, data);
}

void query_id(int client_socket){
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("query");
	writer.String("id");
	writer.Key("id");
	writer.Uint(socket_id[client_socket]);
	writer.EndObject();
	string data(sb.GetString());
	my_send(client_socket, data);
}

void parse(string &data, int client_socket){
	rapidjson::Document doc;
	if (doc.Parse(data.data()).HasParseError()) {
		cerr << "json parse error" << endl;
		return;
	}
	if (doc.HasMember("query") && doc["query"].IsObject()) {
		const rapidjson::Value &object = doc["query"];
		if (object.HasMember("name") && object["name"].IsString()) {
			query_name(client_socket, object);
		} else if (object.HasMember("password") && object["password"].IsString()) {
			query_password(client_socket, object);
		} 
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
	} else if (doc.HasMember("message") && doc["message"].IsString() && doc["message"].GetString() == string("please")) {
		unsigned int id = socket_id[client_socket];
		while (!contacts_mq[id].empty()) {
			string data(contacts_mq[id].front());
			contacts_mq[id].pop();
			my_send(client_socket, data);
		}
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
	}
}

void *client_thread(void *_client_socket){
	char data[MAXSIZE];
	int client_socket = *(int*)_client_socket;
	unsigned int len = 0;
	while(1){
		memset(data, 0, sizeof(data));
		//int ret = recv(client_socket, data, sizeof(data), 0);
		int ret = recv(client_socket, &len, sizeof(len), 0);
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
		//if(!recv(client_socket, message, sizeof(message), 0)){
		//	break;
		//}
		ret = recv(client_socket, data, len, 0);
		pdebug << "recv " << ret << endl;
		string s(data);
		pdebug << data << endl;
		parse(s,client_socket);
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
	string name;
	string message;
	while(1){
		cin >> name;
		getline(cin, message);
		//while(getchar() != '\n');
		message.erase(0,1);
		//send(name_socket[name], message.c_str(), message.length(), 0);
		my_send(name_socket[name], message);
	}
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
	listen(server_sockfd, 4);

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

		pthread_t tid;
		pthread_create(&tid, &attr, client_thread, &client_socket);
	}

	while(1);

	close(server_sockfd);
}
