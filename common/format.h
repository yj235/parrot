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
};
