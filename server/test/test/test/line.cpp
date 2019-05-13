#include <stdio.h>

int main(){
	char s[64] = {0};
	fgets(s, sizeof(s), stdin);
	printf("***%s***\n", s);
}
