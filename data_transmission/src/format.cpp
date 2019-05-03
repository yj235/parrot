#include "../include/format.h"
#include "../include/KVP.h"
#include "../../tools/include/pdebug.h"

#include <iostream>

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

void init(string &sin, string::iterator &i1, string::iterator &i2){
	i1 = i2 = sin.begin();
	next(i2);
}

void analysis_2(string::iterator &i1, string::iterator &i2, KVP *&p){
	if(*i1 == '{' && *i2 == '}'){
		string key(i1 + 1, i1 + string(i1, i2).find(' '));
		string value(i1 + string(i1, i2).find(' ') + 1, i2);
		p = new KVP(key, value);
		pdebug << p->key << " " << p->value << endl;
		next(i1);
		next(i2);

	}
	if(*i1 == '{' && *i2 == '{'){
		p = new KVP(string(i1 + 1, i2));
		pdebug << p->key << endl;
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

void analysis(string &s, KVP *&p){
	string::iterator i1, i2;
	init(s, i1, i2);
	analysis_2(i1, i2, p);
}
