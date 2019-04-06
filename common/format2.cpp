#include "format.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string sout;

void format(KVP *k){
	if(!k->value.empty() || k->next || k->sub){
		sout += '{';
	}
	if(!k->key.empty()){
		sout += k->key;
	}
	if(!k->value.empty()){
		sout += ' ' + k->value + '}';
	}
	if(k->sub){
		format(k->sub);
		sout += '}';
	}
	if(k->next){
		format(k->next);
	} 
}

void next(char **p){
	do{
		++(*p);
		if(**p == '\0'){
			break;
		}
	}while(**p != '{' && **p != '}');
}

char in[256] = {0};
char *p1, *p2;
string sin;

void init(void){
	p1 = p2 = in;
	next(&p2);
}

void p_read(KVP *&p){
	if(*p1 == '{' && *p2 == '}'){
		string key(p1 + 1, strchr(p1, ' '));
		string value(strchr(p1, ' ') + 1, p2);
		p = new KVP(key, value);
		cout << p->key << " " << p->value << endl;
		next(&p1);
		next(&p2);
	}
	if(*p1 == '{' && *p2 == '{'){
		p = new KVP(string(p1 + 1, p2));
		cout << p->key << endl;
		next(&p1);
		next(&p2);
		p_read(p->sub);
	}
	if(*p1 == '}' && *p2 == '}'){
		next(&p1);
		next(&p2);
		return;
	}
	if(*p1 = '}' && *p2 == '{'){
		next(&p1);
		next(&p2);
		p_read(p->next);
		return;
	}
}

int main(int argc, char* argv[]){

	//读测试 c
	//KVP *p;
	//FILE *fp = fopen("data", "r");
	//fread(in, 1, sizeof(in), fp);
	//init();
	//p_read(p);
	//fclose(fp);
	
	//读测试 cpp
	KVP *p;
	ifstream fs("data");
	getline(fs, sin);
	strcpy(in, sin.c_str());
	init();
	p_read(p);
	fs.close();

	//写测试 cpp
	//KVP k1("k1");
	//KVP k2("k2","2");
	//KVP k3("k3","3");

	//k1.sub = &k2;
	//k2.next = &k3;

	//format(&k1);

	//cout << "***" << sout << endl;
	//fstream fs;
	//fs.open("data", ios::out);
	//fs << sout;
	//fs.close();

	return 0;
}
