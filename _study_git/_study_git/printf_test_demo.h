#include <stdio.h>

class PRINTER {
public:
	static char* PRINT_test(char*);	
	static char* PRINT_INT(int, char*);
	static char* PRINT_CHAR(char*, char*);
	//static char* PRINT_LONG(long, char*);
	static char* PRTNT_FLOAT(float, char*);
	static char* PRINT_DOUBLE(double, char*);
	static char* PRINT_CHINESE(char*, char*);
};

char* PRINTER::PRINT_test(char* get) {
	char temp[BUFSIZ];
	snprintf(temp, sizeof(temp), get);
	return temp;
}

char* PRINTER::PRINT_INT(int get, char* temp) {
	snprintf(temp, sizeof(temp), "%d", get);
	return temp;
}

char* PRINTER::PRINT_CHAR(char* get, char* temp) {
	snprintf(temp, sizeof temp, get);
	return temp;
}

//char* PRINTER::PRINT_LONG(long get, char* temp) {
//	snprintf(temp, sizeof(temp),"%l", get);
//	return temp;
//}

char* PRINTER::PRTNT_FLOAT(float get, char* temp) {
	snprintf(temp, sizeof temp, "%f", get);
	return temp;
}

char* PRINTER::PRINT_DOUBLE(double get, char* temp) {
	snprintf(temp, sizeof temp, "%lf", get);
	return temp;
}

char* PRINTER::PRINT_CHINESE(char* get, char* temp) {
	snprintf(temp, sizeof temp, get);
	return temp;
}