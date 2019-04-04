#include <stdio.h>
#include <string.h>

#include <iostream>
#include <string>
#include <stack>

using namespace std;

class KVP{
	public:
		P* next{nullptr};
		P* sub{nullptr};
		string key;
		string value;

		P() = default;
		P(string _key) : key(_key){}
		P(string _key, string _value) : key(_key), value(_value){}
};

//char ps[256] = {0};
//char *pp = ps;
//
//void format(P *p){
//	if(!p->value.empty() || p->next || p->sub){
//		*pp++ = '{';
//	}
//	if(!p->key.empty()){
//		strncpy(pp, p->key.c_str(), p->key.length());
//		pp += p->key.length();
//	}
//	if(!p->value.empty()){
//		strncpy(pp, p->value.c_str(), p->value.length());
//		pp += p->value.length();
//		*pp++ = '}';
//	}
//	if(p->sub){
//		format(p->sub);
//		*pp++ = '}';
//	}
//	if(p->next){
//		format(p->next);
//	} 
//}

char kvp[256] = "{p00{p01{p02{p03 03}{p13{p14 14}{p24 24}}{p33{p34 34}}}}}{p10 10}{p20{p21 21}{p31 31}{p41{p42 42}}}{p50{p51 51}}";

void next(char **p){
	do{
		++(*p);
		if(**p == '\0'){
			break;
		}
	}while(**p != '{' && **p != '}');
}

char *p1, *p2;

void init(void){
	p1 = p2 = &kvp[0];
	next(&p2);
}

void p_read(P *&p){
	if(*p1 == '{' && *p2 == '}'){
		string key(p1 + 1, strchr(p1, ' '));
		string value(strchr(p1, ' ') + 1, p2);
		p = new P(key, value);
		cout << p->key << " " << p->value << endl;
		next(&p1);
		next(&p2);
	}
	if(*p1 == '{' && *p2 == '{'){
		p = new P(string(p1 + 1, p2));
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

	//P p00,p01,p02,p03,p13,p14,p24,p30,p33,p34,p10,p20,p21,p31,p41,p42,p50,p51;

	//p00.key = "p00";
	//p01.key = "p01";
	//p02.key = "p02";
	//p03.key = "p03";
	//p13.key = "p13";
	//p14.key = "p14";
	//p24.key = "p24";
	//p30.key = "p30";
	//p33.key = "p33";
	//p34.key = "p34";
	//p10.key = "p10";
	//p20.key = "p20";
	//p21.key = "p21";
	//p31.key = "p31";
	//p41.key = "p41";
	//p42.key = "p42";

	//p03.value = "_03";
	//p10.value = "_10";
	//p14.value = "_14";
	//p21.value = "_21";
	//p24.value = "_24";
	//p30.value = "_30";
	//p31.value = "_31";
	//p34.value = "_34";
	//p42.value = "_42";

	//p00.sub = &p01;
	//p01.sub = &p02;
	//p02.sub = &p03;
	//p13.sub = &p14;
	//p33.sub = &p34;
	//p20.sub = &p21;
	//p41.sub = &p42;

	//p00.next = &p10;
	//p10.next = &p20;
	//p20.next = &p30;
	//p21.next = &p31;
	//p31.next = &p41;
	//p03.next = &p13;
	//p13.next = &p33;
	//p14.next = &p24;

	//format(&p00);

	//cout << ps << endl;

	P *p;
	init();
	p_read(p);

	cout << p->key << " " << p->next->key << endl;

	return 0;
}
