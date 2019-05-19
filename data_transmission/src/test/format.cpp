//为什么不可以
//#include "~/a/parrot/data_transmission/include/test/format.h"
//#include "~/a/parrot/data_transmission/include/test/KVP.h"
//#include "~/a/parrot/tools/include/pdebug.h"

#include "../../include/test/KVP.h"
#include "../../include/test/format.h"
#include "../../../tools/include/pdebug.h"

#include <iostream>

using namespace std;

//改名了 逐步替换中
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

void kvp_to_string(string &s, KVP *k){
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
		kvp_to_string(s, k->sub);
		s += '}';
	}
	if(k->next){
		kvp_to_string(s, k->next);
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

void init(string &sin, string::iterator &i1, string::iterator &i2){
	i1 = i2 = sin.begin();
	next(i2);
}

void analysis_real(string::iterator &i1, string::iterator &i2, KVP *&p){
	if(*i1 == '{' && *i2 == '}'){
		string key(i1 + 1, i1 + string(i1, i2).find(' '));
		string value(i1 + string(i1, i2).find(' ') + 1, i2);
		p = new KVP(key, value);
		pdebug << p->key << " " << p->value << endl;
		next(i1);
		next(i2);

	}
	//有问题 有sub的不能有value
	//是不是问题?
	if(*i1 == '{' && *i2 == '{'){
		p = new KVP(string(i1 + 1, i2));
		pdebug << p->key << endl;
		next(i1);
		next(i2);
		analysis_real(i1, i2, p->sub);
	}
	if(*i1 == '}' && *i2 == '}'){
		next(i1);
		next(i2);
		return;
	}
	if(*i1 == '}' && *i2 == '{'){
		next(i1);
		next(i2);
		analysis_real(i1, i2, p->next);
		return;
	}
}

void string_to_kvp_real(string::iterator &i1, string::iterator &i2, KVP *&p){
	if(*i1 == '{' && *i2 == '}'){
		string key(i1 + 1, i1 + string(i1, i2).find(' '));
		string value(i1 + string(i1, i2).find(' ') + 1, i2);
		p = new KVP(key, value);
		pdebug << p->key << " " << p->value << endl;
		next(i1);
		next(i2);

	}
	//有问题 有sub的不能有value
	//是不是问题?
	if(*i1 == '{' && *i2 == '{'){
		p = new KVP(string(i1 + 1, i2));
		pdebug << p->key << endl;
		next(i1);
		next(i2);
		string_to_kvp_real(i1, i2, p->sub);
	}
	if(*i1 == '}' && *i2 == '}'){
		next(i1);
		next(i2);
		return;
	}
	if(*i1 == '}' && *i2 == '{'){
		next(i1);
		next(i2);
		string_to_kvp_real(i1, i2, p->next);
		return;
	}
}

void analysis(string &s, KVP *&p){
	string::iterator i1, i2;
	init(s, i1, i2);
	analysis_real(i1, i2, p);
}

void string_to_kvp(string &s, KVP *&p){
	string::iterator i1, i2;
	init(s, i1, i2);
	string_to_kvp_real(i1, i2, p);
}

void string_to_kvp(const char *_s, KVP *&p){
	string s(_s);
	string_to_kvp(s, p);
}
