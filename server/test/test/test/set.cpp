#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

int main(){
	unordered_map<int, unordered_set<int>> room;
	room[3] = {1,2,3};
	room[4];
	room[5] = {4,5};
	for(auto &v : room){
		cout << v.first << ":";
		for(auto &v2 : v.second){
			cout << v2 << " ";
		}
		cout << endl;
	}
}
