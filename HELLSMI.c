#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int decodeNum(char *numstr){
	int num = 0;
	int siz = strlen(numstr);
	for (int p = 0; p < siz; p++){
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

void cutStr(char *newstr, const char *str, int strt){
	for (int pos = 0; pos < 100; pos++){
		if (str[strt+pos] == '\r' || str[strt+pos] == '\n' ||
		(str[strt+pos] == '0' && str[strt+pos+1] == ')')){
			newstr[pos] = '\0';
			return;
		}
		newstr[pos] = str[strt+pos];
	}
	return;
}

void mathOp(const char *cstr, int *PROGMEM, int type){
	char value[100];
	char value2[100];
	char value3[100];
	cutStr(value, cstr, 4);
	cutStr(value2, cstr, strlen(value)+6);
	cutStr(value3, cstr, strlen(value)+strlen(value2)+8);
	switch(type){
		case 0:
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)] + PROGMEM[decodeNum(value3)];
			return;
		case 1:
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)] - PROGMEM[decodeNum(value3)];
			return;
		case 2:
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)] * PROGMEM[decodeNum(value3)];
			return;
		case 3:
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)] / PROGMEM[decodeNum(value3)];
			return;
		case 4:
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)] % PROGMEM[decodeNum(value3)];
			return;
	}
}

int check(int v1, int v2, const char *typ){
	if (!strcmp(typ, "9")){
		if (v1 == v2)
			return 1;
	}
	else if (!strcmp(typ, "0")){
		if (v1 != v2)
			return 1;
	}
	else if (!strcmp(typ, "(")){
		if (v1 < v2)
			return 1;
	}
	else if (!strcmp(typ, ")")){
		if (v1 > v2)
			return 1;
	}
	else if (!strcmp(typ, "((")){
		if (v1 <= v2)
			return 1;
	}
	else if (!strcmp(typ, "))")){
		if (v1 >= v2)
			return 1;
	}
	return 0;
}

void chkCycle(FILE *input, const char *ostr, int *PROGMEM, const char *cod1, const char *cod2){
	char value[100];
	char value2[100];
	char value3[100];
	char cstr[300];
	char cmd[5] = {'\0'};
	cutStr(value, ostr, 4);
	cutStr(value2, ostr, strlen(value)+6);
	cutStr(value3, ostr, strlen(value)+strlen(value2)+10);
	if (!check(PROGMEM[decodeNum(value)], PROGMEM[decodeNum(value2)], value3)){
		int lvl = 1;
		while (lvl > 0){
			if (feof(input)){
				puts("\nERROR: Reached EOF while looking for end of cycle/condition!");
				exit(1);
			}
			fgets(cstr, 300, input);
			memcpy(cmd, cstr, 4 * sizeof(char));
			if (!strcmp(cmd, cod1))
				lvl++;
			else if (!strcmp(cmd, cod2))
				lvl--;
		}
	}
}

void revStr(FILE *input){
	fseek(input, -3, SEEK_CUR);
	while (fgetc(input) != '\n'){
		fseek(input, -2, SEEK_CUR);
		if (ftell(input) == 0){
			printf("\nERROR: Cycle operator is not found!\n");
			exit(1);
		}
	}
	return;
}

int main(int argc, char *argv[]){
    if (argc < 2){
		puts("HELLSMI C Interpreter v1.0");
		puts("Syntax: HELLSMI <Script file>");
		return 0;
	}
	FILE *input;
	if ((input = fopen(argv[1], "rb")) == NULL){
        puts("ERROR: Can't open file!");
		return 1;
	}
	char cstr[300];
	char value[100];
	char value2[100];
	char value3[100];
	char cmd[5] = {'\0'};
	int *PROGMEM;
	fgets(cstr, 300, input);
	memcpy(cmd, cstr, 4 * sizeof(char));
	if (strcmp(cmd, "()90") == 0){
		cutStr(value, cstr, 4);
		int allocs = decodeNum(value);
		if (allocs < 1){
			puts("\nERROR: Can't allocate less than 1 memory block!");
			return 1;
		}
		PROGMEM =(int*)calloc(allocs, sizeof(int));
		if (PROGMEM==NULL){
			puts("\nERROR: Can't allocate memory!");
			return 1;
		}
	}
	else{
		puts("\nERROR: Init string is not found!");
		return 1;
	}
	while (1){
		if (feof(input)) 
			break;
		fgets(cstr, 300, input);
		memcpy(cmd, cstr, 4 * sizeof(char));
		if (!strcmp(cmd, "0000")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			PROGMEM[decodeNum(value)]-= decodeNum(value2);
		}
		else if (!strcmp(cmd, "0009")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			PROGMEM[decodeNum(value)] = decodeNum(value2);
		}
		else if (!strcmp(cmd, "0090")){
			break;
		}
		else if (!strcmp(cmd, "0099")){
			mathOp(cstr, PROGMEM, 0);
		}
		else if (!strcmp(cmd, "0900")){
			mathOp(cstr, PROGMEM, 1);
		}
		else if (!strcmp(cmd, "0909")){
			mathOp(cstr, PROGMEM, 2);
		}
		else if (!strcmp(cmd, "0990")){
			mathOp(cstr, PROGMEM, 3);
		}
		else if (!strcmp(cmd, "0999")){
			mathOp(cstr, PROGMEM, 4);
		}
		else if (!strcmp(cmd, "9000")){
			cutStr(value, cstr, 4);
			printf("%d", PROGMEM[decodeNum(value)]);
		}
		else if (!strcmp(cmd, "9009")){
			cutStr(value, cstr, 4);
			printf("%c", PROGMEM[decodeNum(value)]);
		}
		else if (!strcmp(cmd, "9090")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			PROGMEM[decodeNum(value)] = PROGMEM[decodeNum(value2)];
		}
		else if (!strcmp(cmd, "9099")){
			rewind(input);
			int chk;
			while(chk != '\n')
                chk = fgetc(input);  
		}
		else if (!strcmp(cmd, "9900")){
			chkCycle(input, cstr, PROGMEM,  "9900", "(09)");
		}
		else if (!strcmp(cmd, "9909")){
			chkCycle(input, cstr, PROGMEM, "9909", "(90)");
		}
		else if (!strcmp(cmd, "9990")){
			int chk;
			cutStr(value, cstr, 4);
			int gt = decodeNum(value)-1;
			rewind(input);
			for (int i = 0; i < gt; i++){
				while(chk != '\n'){
					chk = fgetc(input);
					if (chk == EOF){
						puts("\nERROR: Reached EOF when jumping!");
						return 1;
					}
				}	
			}
		}
		else if (!strcmp(cmd, "9999")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			PROGMEM[decodeNum(value)]+= decodeNum(value2);
		}
		else if (!strcmp(cmd, "0()0")){
			cutStr(value, cstr, 4);
			printf("?>");
			scanf("%d", &PROGMEM[decodeNum(value)]);
		}
		else if (!strcmp(cmd, "0)(0")){
			cutStr(value, cstr, 4);
			printf("A?>");
			scanf("%lc", &PROGMEM[decodeNum(value)]);
		}
		else if (!strcmp(cmd, "9((9")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			int strt = decodeNum(value);
			int total = decodeNum(value2) - strt + 1;
			for (int i = 0; i < total; i++){
				printf("%d ", PROGMEM[strt+i]);
			}
			putchar('\n');
		}
		else if (!strcmp(cmd, "9()9")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			int strt = decodeNum(value);
			int total = decodeNum(value2) - strt + 1;
			printf("S?>");
			for (int i = 0; i < total; i++){
				scanf("%d", &PROGMEM[strt+i]);
			}
		}
		else if (!strcmp(cmd, "9)(9")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			int strt = decodeNum(value);
			int total = decodeNum(value2) - strt + 1;
			printf("SA?>");
			for (int i = 0; i < total; i++){
				scanf("%lc", &PROGMEM[strt+i]);
			}
		}
		else if (!strcmp(cmd, "9))9")){
			cutStr(value, cstr, 4);
			cutStr(value2, cstr, strlen(value)+6);
			int strt = decodeNum(value);
			int total = decodeNum(value2) - strt + 1;
			for (int i = 0; i < total; i++){
				printf("%c", PROGMEM[strt+i]);
			}
			putchar('\n');
		}
		else if (!strcmp(cmd, "(09)")){
			int lvl = 1;
			while (lvl > 0){
				revStr(input);
				revStr(input);
				fgets(cstr, 300, input);
				memcpy(cmd, cstr, 4 * sizeof(char));
				if (!strcmp(cmd, "(09)"))
					lvl++;
				else if (!strcmp(cmd, "9900"))
					lvl--;
			}
			revStr(input);
		}
	}
	free(PROGMEM);
	fclose(input);
    return 0;
}