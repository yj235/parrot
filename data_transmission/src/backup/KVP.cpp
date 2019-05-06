#include "../include/KVP.h"

#include <iostream>

using namespace std;

KVP::~KVP(){
	if(this->sub){
		delete this->sub;
	}
	if(this->next){
		delete this->next;
	}
	cout << "KVP destructor" << endl;
}
