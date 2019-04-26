#include <iostream>
#include <string>

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

void* thread_send(void* _client_sockfd){
	char buffer[32] = {0};
	int client_sockfd = *(int*)_client_sockfd;
	while(1){
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0';
		send(client_sockfd, buffer, strlen(buffer), 0);
	}
}

void* thread_recv(void* _client_sockfd){
	char buffer[8192] = {0};
	int client_sockfd = *(int*)_client_sockfd;
	while(1){
		memset(buffer, 0, sizeof(buffer));
		if(!recv(client_sockfd, buffer, sizeof(buffer), 0)){
			break;
		}
		printf("%s", buffer);
		fflush(stdout);
	}
}

//void string_analysis2(int client_sockfd, char* message){
//	if(!strncmp(message, "login", strlen("login"))){
//		if(!strncmp(message, "login|name ", strlen("login|name "))){
//			
//		}
//	} else if (!strncmp(message, "regist", strlen("regist"))){
//		
//	}
//}
//
//void string_analysis(int client_sockfd, char* message){
//	char key[32] = {0}, value[32] = {0};
//	sscanf(message, "%s %s", key, value);
//	int ret = 0;
//	static char name[32] = {0}, password[32] = {0};
//	if(!strcmp(key, "name")){
//		FILE* fp = fopen("user_info", "a+");
//		while(fscanf(fp, "%s %s", name, password) != EOF){
//			if(!strcmp(value, name)){
//				ret = 1;
//				send(client_sockfd, &ret, sizeof(int), 0);
//				return;
//			}
//		}
//		ret = 0;
//		send(client_sockfd, &ret, sizeof(int), 0);
//	} else if (!strcmp(key, "password")) {
//		if(!strcmp(value, password)){
//			ret = 1;
//			send(client_sockfd, &ret, sizeof(int), 0);
//		} else {
//			ret = 0;
//			send(client_sockfd, &ret, sizeof(int), 0);
//		}
//	}
//}

//void string_analysis(int client_sockfd, char* message) {
//	static char name[32] = {0}, password[32] = {0};
//	int ret = 0;
//	if (!strncmp(message, "name ", strlen("name "))) {
//		FILE* fp = fopen("user_info", "a+");
//		char buf[64] = {0};
//		while (fgets(buf, sizeof(buf), fp)) {
//			buf[strlen(buf) - 1] = '\0';
//			if (!strncmp(buf, message + strlen("name "), strlen(message + strlen("name ")))) {
//				strncpy(name, buf, strchr(buf, ' ') - buf);
//				strcpy(password, strchr(buf, ' ') + 1);
//				ret = 1;
//				send(client_sockfd, &ret, sizeof(int), 0);
//				return;
//			}
//		}
//		ret = 0;
//		send(client_sockfd, &ret, sizeof(int), 0);
//	} else if (!strncmp(message, "password ", strlen("password "))) {
//		if (!strcmp(password, message + strlen("password "))) {
//			ret = 1;
//			send(client_sockfd, &ret, sizeof(int), 0);
//		} else {
//			ret = 0;
//			send(client_sockfd, &ret, sizeof(int), 0);
//		}
//	}
//}

int main(){
	unsigned short port = 8080;
	struct sockaddr_in server_sockaddr;

	int server_sock = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_sockaddr, 0, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(port);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(server_sock, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr));
	listen(server_sock, 2);

	struct sockaddr_in client_sockfdaddr;
	socklen_t client_sockaddr_len = sizeof(client_sockfdaddr);
	int client_sockfd = accept(server_sock, (struct sockaddr*)&client_sockfdaddr, &client_sockaddr_len);

	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, thread_recv, &client_sockfd);
	pthread_create(&tid2, NULL, thread_send, &client_sockfd);

	//pthread_join(tid1, NULL);
	while(1);

	//char buf[128] = {0};
	//while(1){
	//	memset(buf, 0, sizeof(buf));
	//	if(!recv(client_sockfd, buf, sizeof(buf), 0)){
	//		break;
	//	}
	//	string_analysis(client_sockfd, buf);
	//}
}
