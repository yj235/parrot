#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(){
	string name("na");
	string sql = "select * from user where name=\"" + name + "\"";
	cout << sql;
}
