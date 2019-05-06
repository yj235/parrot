#include "KVP.h"
#include "format.h"
#include "User.h"
#include "pdebug.h"

#include "my_query.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>

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

vector<int> socket_vector;
map<int, int> id_socket;

void parsing(string &s, KVP *&p){
	analysis(s, p);
	if(p->key == "login"){
		//string password("password");
		User u(p->find("name")->value, p->find("password")->value);
		pdebug << u << endl;
		string sql = "select id from user where name=\"" + u.name + "\" and password=\"" + u.password + "\"";
		pdebug << sql << endl;
		if(my_query(sql.c_str())){
			pdebug << "successed" << endl;
		} else {
			pdebug << "failed" << endl;
		}
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
		//KVP *p;
		//parsing(s, p);
		pdebug << message << endl;

		//delete p;
	}
}

void* server_thread(void* _client_sockfd){
	char buffer[32] = {0};
	while(1){
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0';
		send(*socket_vector.begin(), buffer, strlen(buffer), 0);
	}
}

int main(){
	unsigned short port = 8080;
	struct sockaddr_in server_sockaddr;

	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//设置调用closesocket()后，仍可继续重用该socket.(调用closesocket()一般不会立即关闭socket,而经历TIME_WAIT的过程.)
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
		socket_vector.push_back(client_sockfd);

		pthread_t tid;
		pthread_create(&tid, &attr, client_thread, &client_sockfd);
	}

	//pthread_join(tid, NULL);

	while(1);

	close(server_sockfd);
	//close(client_sockfd);
}
