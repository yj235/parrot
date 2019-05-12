#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

int main(){
	string s("hello world");
	send(4, s.c_str(), s.length(), 0);
}
