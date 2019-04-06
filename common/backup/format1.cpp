#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <stack>

using namespace std;

class KVP{
	public:
		KVP* next{nullptr};
		KVP* sub{nullptr};
		string key;
		string value;

		KVP() = default;
		KVP(string _key) : key(_key){}
		KVP(string _key, string _value) : key(_key), value(_value){}
};


char out[256] = {0};
char *op = out;

void format(KVP *k){
	if(!k->value.empty() || k->next || k->sub){
		*op++ = '{';
	}
	if(!k->key.empty()){
		strncpy(op, k->key.c_str(), k->key.length());
		op += k->key.length();
	}
	if(!k->value.empty()){
		strncpy(op, k->value.c_str(), k->value.length());
		op += k->value.length();
		*op++ = '}';
	}
	if(k->sub){
		format(k->sub);
		*op++ = '}';
	}
	if(k->next){
		format(k->next);
	} 
}

//char tp[256] = "{p00{p01{p02{p03 03}{p13{p14 14}{p24 24}}{p33{p34 34}}}}}{p10 10}{p20{p21 21}{p31 31}{p41{p42 42}}}{p50{p51 51}}";

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

	//KVP p00,p01,p02,p03,p13,p14,p24,p30,p33,p34,p10,p20,p21,p31,p41,p42,p50,p51;

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
	//p50.key = "p50";
	//p51.key = "p51";

	//p03.value = " 03";
	//p10.value = " 10";
	//p14.value = " 14";
	//p21.value = " 21";
	//p24.value = " 24";
	//p30.value = " 30";
	//p31.value = " 31";
	//p34.value = " 34";
	//p42.value = " 42";
	//p51.value = " 51";

	//p00.sub = &p01;
	//p01.sub = &p02;
	//p02.sub = &p03;
	//p13.sub = &p14;
	//p33.sub = &p34;
	//p20.sub = &p21;
	//p41.sub = &p42;
	//p50.sub = &p51;

	//p00.next = &p10;
	//p10.next = &p20;
	//p20.next = &p30;
	//p21.next = &p31;
	//p31.next = &p41;
	//p03.next = &p13;
	//p13.next = &p33;
	//p14.next = &p24;
	//p20.next = &p50;

	//format(&p00);

	//cout << out << endl;
	
	//写测试
	KVP k1("k1");
	KVP k2("k2","2");
	KVP k3("k3","3");

	k1.sub = &k2;
	k2.next = &k3;

	format(&k1);

	cout << out << endl;

	//FILE *fp = fopen("trans", "wb");
	//fwrite(out, 1, sizeof(out), fp);

	//fclose(fp);


	//KVP *p;
	//FILE *fp = fopen("trans", "r");
	//fread(in, 1, sizeof(in), fp);
	//printf("---%s\n", in);
	//init();
	//p_read(p);

	
	//FILE *fp = fopen("trans", "r");
	//char tp[256] = {0};
	//fread(tp, 1, sizeof(tp), fp);
	//init(tp);
	//KVP *k = nullptr;
	//p_read(k);

	return 0;
}
