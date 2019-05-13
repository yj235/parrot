#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main(){
	unordered_map<string, string> um;
	//um.insert({"hello", "world"});
	//um.insert({"ni", "hao"});
	//um.insert({"good", "bye"});

	um["1"] = "a";
	um["2"] = "b";
	um["3"] = "c";

	for(auto &v : um){
		cout << v.first << " " << v.second << endl;
	}

	cout << um["hello"] << endl;

	for(auto &v : um){
		cout << v.first << " " << v.second << endl;
	}
}
