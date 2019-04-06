#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(){
	string s("12 34");
	fstream fs;
	fs.open("data", ios::out);
	fs << s << endl;
	fs.close();
}
