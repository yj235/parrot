#include <iostream>
#include <string>

using namespace std;

int main(){
	string name;
	string message;
	cin >> name;
	getline(cin, message);
	cout << "***" << name << "***" << endl;
	cout << "***" << message << "***" << endl;
}
