#include "KVP.h"
#include "format.h"
#include "User.h"
#include "pdebug.h"

#include "my_query.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
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

//vector<int> socket_vector;
unordered_map<string, int>user_socket;

void parsing(string &s, KVP *&p, int client_sockfd){
	analysis(s, p);
	if(p->key == "login"){
		User u(p->find("name")->value, p->find("password")->value);
		pdebug << u << endl;
		string sql = "select id from user where name=\"" + u.name + "\" and password=\"" + u.password + "\"";
		pdebug << sql << endl;
		char*p = my_query(sql.c_str());
		if(p){
			pdebug << "successed" << endl;
			pdebug << p << endl;
			user_socket.insert({u.name, client_sockfd});
			for(auto &v: user_socket){
				cout << v.first << " " << v.second << endl;
			}
		} else {
			pdebug << "failed" << endl;
		}
	} else if ("send" == p->key){
		//pdebug << "shide" << endl;
		KVP *obj = p->sub;
		send(user_socket[obj->key], obj->value.c_str(), obj->value.length(), 0);
		//send(user_socket["na"], "nihao", strlen("nihao"), 0);
	}
}

void *client_thread(void *_client_sockfd){
	char message[64] = {0};
	int client_sockfd = *(int*)_client_sockfd;
	while(1){
		memset(message, 0, sizeof(message));
		if(!recv(client_sockfd, message, sizeof(message), 0)){
			break;
		}
		string s(message);
		KVP *p;
		parsing(s, p, client_sockfd);
		pdebug << message << endl;

		delete p;
	}
}

void* server_thread_0(void* _client_sockfd){
	char buffer[32] = {0};
	string name;
	while(1){
		cin >> name;
		while(getchar() != '\n');
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0';
		//send(*socket_vector.begin(), buffer, strlen(buffer), 0);
		send(user_socket["na"], buffer, strlen(buffer), 0);
	}
}

void* server_thread(void* _client_sockfd){
	string name;
	string message;
	while(1){
		cin >> name >> message;
		while(getchar() != '\n');
		send(user_socket[name], message.c_str(), message.length(), 0);
	}
}

void* server_thread_2(void* _client_sockfd){
	char buffer[32] = {0};
	while(1){
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0';
		send(*(int*)_client_sockfd, buffer, strlen(buffer), 0);
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
		struct sockaddr_in client_sockfdaddr;
		socklen_t client_sockaddr_len = sizeof(client_sockfdaddr);

		int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_sockfdaddr, &client_sockaddr_len);

		//添加客户端socket到vector
		//socket_vector.push_back(client_sockfd);

		pthread_t tid;
		pthread_create(&tid, &attr, client_thread, &client_sockfd);
	}

	//pthread_join(tid, NULL);

	while(1);

	close(server_sockfd);
	//close(client_sockfd);
}
