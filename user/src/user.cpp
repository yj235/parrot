#include "user.h"
#include "../../tools/include/pdebug.h"

using namespace std;

User::~User(){
	pdebug << name << " destructor" << endl;
}

std::ostream& operator<<(std::ostream& out, User& u){
	pdebug << u.name << " " << u.password;
	return out;
}
