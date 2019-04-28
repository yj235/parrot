#ifndef FORMAT_H_
#define FORMAT_H_

#include <string>
#include <iostream>
#include <iterator>

class KVP{
	public:
		std::string key;
		std::string value;
		KVP *sub{nullptr};
		KVP *next{nullptr};

		KVP(){}
		KVP(std::string _key) : key(_key){}
		KVP(std::string _key, std::string _value) : key(_key), value(_value){}
};

extern std::string sin;
extern std::string::iterator i1, i2;

void format(std::string &s, KVP *k);
void next(std::string::iterator &i);
void init(void);
void p_read(KVP *&p);

#endif
