#include <string>
#include <iostream>
#include <unordered_set>

typedef struct ID_Switch{
	unsigned int id;
	bool controller;
}ID_Switch;

class User{
	public:
		unsigned int id;
		std::string name;
		std::string password;
		std::unordered_set<int> sockets;
		ID_Switch contacts;
		ID_Switch group;

		User(std::string _name, std::string _password) : name(_name), password(_password){}
		User(unsigned int _id, std::string _name) : id(_id), name(_name){}
		User(unsigned int _id, std::string _name, std::string _password) : id(_id), name(_name), password(_password){}

		~User();
};

std::ostream& operator<<(std::ostream& out, User& u);
