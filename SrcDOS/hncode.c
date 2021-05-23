#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void encodeNum(int number, char* str){
	int nums[10];
	int siz = 0;
	int strp = 0;
	int p, n;
	if (number == 0){
		strcpy(str, ")0)");
		return;
	}
	else if (number < 0){
		strcpy(str, ")0");
		number*= -1;
		strp+=2;
	}
	while (number != 0){
		nums[siz] = number % 10;
		number/= 10;
		siz++;
	}
	for (p = siz-1; p >= 0; p--){
		if (nums[p] == 0){
			str[strp] = ')';
			strp++;
		}
		else{
			for (n = nums[p]; n > 0; n--){
				str[strp] = '(';
				strp++;
			}	
		}
		str[strp] = '9';
		strp++;
	}
	str[strp-1] = '0';
	str[strp] = ')';
	return;
}

int decodeNum(char *numstr){
	int num = 0;
	int siz = strlen(numstr);
	int p;
	for (p = 0; p < siz; p++){
		if (numstr[p] == '(')
			num++;
		else if (numstr[p] == '9')
			num*= 10;
	}
	if (numstr[0] == ')' && numstr[1] == '0'){
		num*= -1;
	}
	return num;
}

int main(int argc, char *argv[]){
    if (argc < 3){
		printf("HELLSMI Number Coder\n");
		printf("Syntax: hncode <-e or -d> <value>\n");
		return 0;
	}
	argv[1][1] = tolower(argv[1][1]);
	if (!strcmp(argv[1], "-e")){
		int codnum = atoi(argv[2]);
		char res[100] = {'\0'};
		encodeNum(codnum, res);
		printf("HELLSMI number is: %s\n", res);
	}
	else if (!strcmp(argv[1], "-d")){
		printf("Numder is: %d\n", decodeNum(argv[2]));
	}
	else{
		printf("Incorrect mode!\n");
	}
    return 0;
}
