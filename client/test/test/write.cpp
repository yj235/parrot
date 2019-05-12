#include <iostream>
#include <string>

#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(){
	write(1, "hello\n", 6);
	string s("world");
	write(1, s.c_str(), s.length());
}
