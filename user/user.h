#include <string>

class User{
	public:
		unsigned int id;
		std::string name;
		std::string password;

		User(std::string _name, std::string _password) : name(_name), password(_password);
}
