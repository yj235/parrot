#include "../include/format.h"

//#include <fstream>

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

void next(string::iterator &i){
	do{
		++i;
		if(*i == '\0'){
			break;
		}
	}while(*i != '{' && *i != '}');
}

//string sin;
//string::iterator i1, i2;

void init(string &sin, string::iterator &i1, string::iterator &i2){
	i1 = i2 = sin.begin();
	next(i2);
}

void analysis_2(string::iterator &i1, string::iterator &i2, KVP *&p){
	if(*i1 == '{' && *i2 == '}'){
		string key(i1 + 1, i1 + string(i1, i2).find(' '));
		string value(i1 + string(i1, i2).find(' ') + 1, i2);
		p = new KVP(key, value);
		cout << p->key << " " << p->value << endl;
		next(i1);
		next(i2);

	}
	if(*i1 == '{' && *i2 == '{'){
		p = new KVP(string(i1 + 1, i2));
		cout << p->key << endl;
		next(i1);
		next(i2);
		analysis_2(i1, i2, p->sub);
	}
	if(*i1 == '}' && *i2 == '}'){
		next(i1);
		next(i2);
		return;
	}
	if(*i1 = '}' && *i2 == '{'){
		next(i1);
		next(i2);
		analysis_2(i1, i2, p->next);
		return;
	}
}

void analysis(string &sin, string::iterator &i1, string::iterator &i2, KVP *&p){
	init(sin, i1, i2);
	analysis_2(i1, i2, p);
}

int main(int argc, char* argv[]){

	//写测试
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

	//读测试
	//KVP *p;
	//ifstream fs("data");
	//string sin;
	//string::iterator i1, i2;
	//getline(fs, sin);
	//analysis(sin, i1, i2, p);
	//fs.close();

	return 0;
}
