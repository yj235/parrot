#include "format.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

using namespace std;

void format(string &s, KVP *k){
	if(!k->value.empty() || k->next || k->sub){
		s += '{';
	}
	if(!k->key.empty()){
		s += k->key;
	}
	if(!k->value.empty()){
		s += ' ' + k->value + '}';
	}
	if(k->sub){
		format(s, k->sub);
		s += '}';
	}
	if(k->next){
		format(s, k->next);
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

void next2(string::iterator &i){
	do{
		++i;
		if(*i == '\0'){
			break;
		}
	}while(*i != '{' && *i != '}');
}

char in[256] = {0};
char *p1, *p2;

string sin;
string::iterator i1, i2;

void init(void){
	p1 = p2 = in;
	next(&p2);
}

void init2(void){
	i1 = i2 = sin.begin();
	next2(i2);
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

void p_read2(KVP *&p){
	if(*i1 == '{' && *i2 == '}'){
		string key(i1 + 1, i1 + string(i1, i2).find(' '));
		string value(i1 + string(i1, i2).find(' ') + 1, i2);
		p = new KVP(key, value);
		cout << p->key << " " << p->value << endl;
		next2(i1);
		next2(i2);

	}
	if(*i1 == '{' && *i2 == '{'){
		p = new KVP(string(i1 + 1, i2));
		cout << p->key << endl;
		next2(i1);
		next2(i2);
		p_read2(p->sub);
	}
	if(*i1 == '}' && *i2 == '}'){
		next2(i1);
		next2(i2);
		return;
	}
	if(*i1 = '}' && *i2 == '{'){
		next2(i1);
		next2(i2);
		p_read2(p->next);
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
	init2();
	p_read2(p);
	fs.close();

	//写测试 cpp
	//KVP k1("k1");
	//KVP k2("k2","2");
	//KVP k3("k3","3");

	//k1.sub = &k2;
	//k2.next = &k3;

	//string s;
	//format(s, &k1);

	//cout << "***" << s<< endl;
	//fstream fs;
	//fs.open("data", ios::out);
	//fs << s;
	//fs.close();

	return 0;
}
