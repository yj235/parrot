#ifndef KVP_H
#define KVP_H

#include <string>

class KVP{
	public:
		std::string key;
		std::string value;
		KVP *sub{nullptr};
		KVP *next{nullptr};

		KVP(){}
		KVP(std::string _key) : key(_key){}
		KVP(std::string _key, std::string _value) : key(_key), value(_value){}
		~KVP();

		KVP* find(std::string& s);
		KVP* find(const char* _s);
};

#endif
