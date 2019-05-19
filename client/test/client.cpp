#include "format.h"
#include "KVP.h"
#include "User.h"
#include "pdebug.h"

#include <iostream>
#include <string>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *t_send(void *fd){
	string head;
	string key;
	string value;
	string data;
	while(true){
		cin >> head >> key;
		getline(cin, value);
		value.erase(0,1);
		data = "{" + head + "{" + key + " " + value + "}}";
		pdebug << data << endl;
		send(*(int*)fd, data.c_str(), data.length(), 0);
	}
}

void *t_recv(void* fd){
	char data[64] = {0};
	while(true){
		memset(data, 0, sizeof(data));
		if(!recv(*(int*)fd, data, sizeof(data), 0)){
			break;
		}
		pdebug << data << endl;
		KVP *p;
		string_to_kvp(data, p);
		if ("query" == p->key) {
			if ("name" == p->sub->key) {
				if ("exist" == p->sub->value) {
					pdebug << "name exist" << endl;
				} else {
					pdebug << "name not exist" << endl;
				}
			} else if ("password" == p->sub->key) {
				if ("correct" == p->sub->value) {
					pdebug << "password correct" << endl;
				} else {
					pdebug << "password incorrect" << endl;
				}
			}
		} else if ("send" == p->key) {
			cout << p->sub->key << " " << p->sub->value << endl;
		} else if ("room" == p->key) {
			cout << p->sub->key << " " << p->sub->value << endl;
		} else {
			cout << "other" << endl;
		}
		if (p) {
			delete p;
		}
	}
}

int main(int argc, char* argv[]){
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	char* s_ip = "192.168.196.169";
	short port = 8080;

	struct sockaddr_in s_sockaddr;
	s_sockaddr.sin_family = AF_INET;
	s_sockaddr.sin_port = htons(port);
	s_sockaddr.sin_addr.s_addr = inet_addr(s_ip);

	while(true){
		if(connect(fd, (struct sockaddr*)&s_sockaddr, sizeof(s_sockaddr))){
			cout << "connection failed! reconnect in 2s" << endl;
			sleep(2);
		} else {
			cout << "connected" << endl;
			break;
		}
	}

	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, t_send, &fd);
	pthread_create(&tid2, NULL, t_recv, &fd);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	close(fd);

	return 0;
}
