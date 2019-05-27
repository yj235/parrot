#include <string>
#include <iostream>

class User{
	public:
		unsigned int id;
		std::string name;
		std::string password;

		User(std::string _name, std::string _password) : name(_name), password(_password){}
		User(unsigned int _id, std::string _name) : id(_id), name(_name){}

		~User();
};

std::ostream& operator<<(std::ostream& out, User& u);
