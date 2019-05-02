#include <string>
#include <iostream>

class User{
	public:
		unsigned int id;
		std::string name;
		std::string password;

		User(std::string _name, std::string _password) : name(_name), password(_password){}

		~User(){
			std::cout << "destructor" << std::endl;
		}
};

std::ostream& operator<<(std::ostream& out, User& u){
	out << u.name << " " << u.password;
	return out;
}
