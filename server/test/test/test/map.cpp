#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main(){
	unordered_map<string, string> um;
	um.insert({"hello", "world"});
	um.insert({"ni", "hao"});
	um.insert({"good", "bye"});

	for(auto &v : um){
		cout << v.first << " " << v.second << endl;
	}

	cout << um["hello"] << endl;
}
