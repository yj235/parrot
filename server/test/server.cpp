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

using namespace std;

//<用户名 端口> 用id? //有bug 登录
unordered_map<string, int>name_socket;
//<端口 用户名> 用id? //有bug 登录
unordered_map<int, string>socket_name;
//<用户名 密码>
unordered_map<string, string>name_password;
//<端口,id>
unordered_map<int, unsigned int>socket_id;
//聊天室 <房间号,socket_fd>
unordered_map<string, unordered_set<int>> room;

//void parsing(string &s, KVP *&p, int client_socket){
//	analysis(s, p);
//	if ("send" == p->key){
//		KVP *obj = p->sub;
//		string data = "{send{" + socket_name[client_socket] + " " + obj->value + "}}";
//		send(name_socket[obj->key], data.c_str(), data.length(), 0);
//	} else if ("room" == p->key){
//		room[p->sub->key].insert(client_socket);
//		string data = "{room{" + socket_name[client_socket] + " " + p->sub->value + "}}";
//		for(auto &v : room[p->sub->key]){
//			send(v, data.c_str(), data.length(), 0);
//		}
//	} else if ("query" == p->key){
//		if ("name" == p->sub->key) {
//			string name = p->sub->value;
//			socket_name[client_socket] = name;
//			string sql = "select password from user where name=\"" + name + "\"";
//			vector<vector<string>> vvs = my_query(sql);
//			//for(auto &v : vvs){
//			//	for(auto &v2 : v){
//			//		pdebug << v2 << endl;
//			//	}
//			//}
//			if(!vvs.empty()){
//				string password(vvs[0][0]);
//				pdebug << password << endl;
//				name_password[name] = password;
//				pdebug << "name exist" << endl;
//				string kvp_query("{query{name exist}}");
//				send(client_socket, kvp_query.c_str(), kvp_query.length(), 0);
//			} else {
//				pdebug << "name not exist" << endl;
//				string kvp_query("{query{name not exist}}");
//				send(client_socket, kvp_query.c_str(), kvp_query.length(), 0);
//			}
//		} else if ("password" == p->sub->key) {
//			string name = socket_name[client_socket];
//			string password = p->sub->value;
//			unordered_map<string, string>::iterator it;
//			it = name_password.find(name);
//			if (name_password.end() != it) {
//				if (password == name_password[name]) {
//					name_socket[name] = client_socket;
//					for (auto &v : name_socket) {
//						pdebug << v.first << " " << v.second << endl;
//					}
//					pdebug << "password correct" << endl;
//					string kvp_query("{query{password correct}}");
//					send(client_socket, kvp_query.c_str(), kvp_query.length(), 0);
//				} else {
//					pdebug << "password incorrect" << endl;
//					string kvp_query("{query{password incorrect}}");
//					send(client_socket, kvp_query.c_str(), kvp_query.length(), 0);
//				}
//			} else {
//				string sql = "insert into user(name, password) values(\"" + name + "\", \"" + password + "\")";
//				string data;
//				if (my_query_int(sql)) {
//					data = "{regist failed}";
//					pdebug << "user insert failed" << endl;
//				} else {
//					sql = "create table " + name + "_relationship(id int unsigned not null auto_increment primary key, contact_id int unsigned not null)";
//					if (my_query_int(sql)) {
//						pdebug << "create relationship failed" << endl;
//					} else {
//						pdebug << "create relationship successed" << endl;
//					}
//					name_socket[name] = client_socket;
//					data = "{regist success}";
//					pdebug << "user insert success" << endl;
//				}
//				send(client_socket, data.c_str(), data.length(), 0);
//			}
//		}
//	} else {
//		pdebug << "other" << endl;
//	}
//}

void parse(string &data, int client_socket){
	rapidjson::Document doc;
	if (doc.Parse(data.data()).HasParseError()) {
		cerr << "json parse error" << endl;
		return;
	}
	if (doc.HasMember("query") && doc["query"].IsObject()) {
		const rapidjson::Value &object = doc["query"];
		if (object.HasMember("name") && object["name"].IsString()) {
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
				string password = vvs[0][1];
				name_password[name] = password;
				writer.String("exist");
			}
			writer.EndObject();
			writer.EndObject();
			string data_send = sb.GetString();
			send(client_socket, data_send.c_str(), data_send.length(), 0);

		} else if (object.HasMember("password") && object["password"].IsString()) {
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
			send(client_socket, data_send.c_str(), data_send.length(), 0);
		} 
	} else if (doc.HasMember("query") && doc["query"].IsString() && doc["query"].GetString() == string("contacts")) {
		string contacts_query = "select user.name from user, " + to_string(socket_id[client_socket]) + "_contacts where user.id=" + to_string(socket_id[client_socket]) + "_contacts.contacts_id";
		vector<vector<string>> vvs(my_query(contacts_query));
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.Key("contacts list");
		writer.StartArray();
		for (auto &v : vvs) {
			for (auto &v2 : v) {
				writer.String(v2.c_str());
			}
		}
		writer.EndArray();
		writer.EndObject();
		string data(sb.GetString());
		pdebug << data << endl;
		send(client_socket, data.c_str(), data.length(), 0);
	} else if (doc.HasMember("regist") && doc["regist"].IsObject()) {
		const rapidjson::Value &object = doc["regist"];
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
			string query_id = "select id from user where name=\"" + name + "\"";
			socket_id[client_socket] = stoul(my_query(query_id)[0][0]);
			string create_contacts = "create table " + to_string(socket_id[client_socket]) + "_contacts(id int unsigned not null auto_increment primary key, contacts_id int unsigned not null)";
			if (my_query_int(create_contacts)) {
				pdebug << "create contacts failed" << endl;
			} else {
				pdebug << "create contacts success" << endl;
			}
			writer.String("success");
			pdebug << "insert user success" << endl;
		}
		writer.EndObject();
		string data = sb.GetString();
		send(client_socket, data.c_str(), data.length(), 0);
	} else if (doc.HasMember("send") && doc["send"].IsObject()) {
		const rapidjson::Value &object = doc["send"];
		string name(object["name"].GetString());
		string data_received(object["data"].GetString());
		//可以修改json 还不会
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		writer.StartObject();
		writer.Key("send");
		writer.StartObject();
		writer.Key("name");
		writer.String(socket_name[client_socket].c_str());
		writer.Key("data");
		writer.String(data_received.c_str());
		writer.EndObject();
		writer.EndObject();
		string data_send(sb.GetString());
		pdebug << data_send << endl;
		send(name_socket[name], data_send.c_str(), data_send.length(), 0);
	}
}

void *client_thread(void *_client_socket){
	char message[64] = {0};
	int client_socket = *(int*)_client_socket;
	while(1){
		memset(message, 0, sizeof(message));
		if(!recv(client_socket, message, sizeof(message), 0)){
			break;
		}
		string s(message);
		pdebug << message << endl;
		//KVP *p;
		//parsing(s, p, client_socket);
		parse(s,client_socket);
		//if (p) {
		//	delete p;
		//}
	}
}

void* server_thread(void* _client_socket){
	string name;
	string message;
	while(1){
		cin >> name;
		getline(cin, message);
		//while(getchar() != '\n');
		message.erase(0,1);
		send(name_socket[name], message.c_str(), message.length(), 0);
	}
}

int main(){
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
