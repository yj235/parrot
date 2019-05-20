//问题 为什么不可以
//#include "~/a/parrot/data_transmission/include/test/KVP.h"
//#include "~/a/parrot/tools/include/pdebug.h"

#include "../../include/test/KVP.h"
#include "../../../tools/include/pdebug.h"

#include <iostream>

using namespace std;

KVP::~KVP(){
	if(this->sub){
		delete this->sub;
	}
	if(this->next){
		delete this->next;
	}
	pdebug << this->key << " destructor" << endl;
}

KVP *KVP::find(string& s){
	if(this->key == s){
		pdebug << "find" << endl;
		return this;
	} else {
		if(this->sub){
			KVP *p = this->sub->find(s);
			if(p != nullptr){
				return p;
			}
		}
		if(this->next){
			KVP *p = this->next->find(s);
			if(p != nullptr){
				return p;
			}
		}
	}
	return nullptr;
}

KVP *KVP::find(const char* _s){
	string s(_s);
	return find(s);
}
