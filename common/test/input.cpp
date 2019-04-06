#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(){
	fstream fs("data", ios::in);
	string s;
	getline(fs, s);
	cout << s << endl;
	fs.close();
}
