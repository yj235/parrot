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

void login(void *fd){
	string name, password;
	cout << "请输入账号";
	cin >> name;
	cout << "请输入密码";
	cin >> password;
	//cin.sync(); //没用
	while(getchar() != '\n');

	KVP *login_kvp = new KVP("login");
	KVP *name_kvp = new KVP("name", name);
	KVP *password_kvp = new KVP("password", password);

	login_kvp->sub = name_kvp;
	name_kvp->next = password_kvp;

	string s;
	format(s, login_kvp);
	pdebug << s << endl;
	send(*(int*)fd, s.c_str(), s.length(), 0);

	delete login_kvp;
}

void *t_send_0(void *fd){
	login(fd);
	char message[64] = {0};
	while(true){
		memset(message, 0, sizeof(message));
		fgets(message, sizeof(message), stdin);
		message[strlen(message) - 1] = '\0';
		send(*(int*)fd, message, strlen(message), 0);
	}
}

//new
void *t_send__0(void *fd){
	login(fd);
	string message;
	while(true){
		getline(cin, message);
		//while(getchar() != '\n');
		//message = "{send{" + message + "}}";
		message = "{room{1 " + message + "}}";
		pdebug << message << endl;
		send(*(int*)fd, message.c_str(), message.length(), 0);
	}
}

void *t_send(void *fd){
	login(fd);
	string head;
	string key;
	string value;
	string message;
	while(true){
		cin >> head >> key;
		getline(cin, value);
		value.erase(0,1);
		//while(getchar() != '\n');
		//message = "{send{" + message + "}}";
		message = "{" + head + "{" + key + " " + value + "}}";
		pdebug << message << endl;
		send(*(int*)fd, message.c_str(), message.length(), 0);
	}
}

void *t_recv(void* fd){
	char message[64] = {0};
	while(true){
		memset(message, 0, sizeof(message));
		if(!recv(*(int*)fd, message, sizeof(message), 0)){
			break;
		}
		pdebug << message << endl << flush;
	}
}

int main(int argc, char* argv[]){
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	char* s_ip = "192.168.196.168";
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

	//while(true){}

	return 0;
}
