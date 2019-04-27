#include <iostream>
//#include <string>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

void* t_send(void* fd){
	char message[64] = {0};
	while(1){
		memset(message, 0, sizeof(message));
		fgets(message, sizeof(message), stdin);
		message[strlen(message) - 1] = '\0';
		send(*(int*)fd, message, strlen(message), 0);
	}
}

void* t_recv(void* fd){
	char message[64] = {0};
	while(1){
		memset(message, 0, sizeof(message));
		if(!recv(*(int*)fd, message, sizeof(message), 0)){
			break;
		}
		printf("%s\n", message);
		fflush(stdout);
	}
}

int main(int argc, char* argv[]){
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	char* s_ip = "192.168.196.162";
	short port = 8080;

	struct sockaddr_in s_sockaddr;
	s_sockaddr.sin_family = AF_INET;
	s_sockaddr.sin_port = htons(port);
	s_sockaddr.sin_addr.s_addr = inet_addr(s_ip);

	connect(fd, (struct sockaddr*)&s_sockaddr, sizeof(s_sockaddr));

	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, t_send, &fd);
	pthread_create(&tid2, NULL, t_recv, &fd);

	while(1){}

	return 0;
}
