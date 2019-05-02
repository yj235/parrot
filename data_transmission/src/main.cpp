#include "../include/format.h"

#include <fstream>

using namespace std;

int main(void){
	//写测试 cpp
	//KVP k1("k1");
	//KVP k2("k2","2");
	//KVP k3("k3","3");

	//k1.sub = &k2;
	//k2.next = &k3;

	//string s;
	//format(s, &k1);

	//cout << s<< endl;
	//fstream fs;
	//fs.open("data", ios::out);
	//fs << s;
	//fs.close();

	//读测试 cpp
	KVP *p;
	ifstream fs("data");
	getline(fs, sin);
	init();
	p_read(p);
	fs.close();

	return 0;
}
