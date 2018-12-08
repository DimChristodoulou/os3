#include "../inc/global.h"

void errCatch(char* errmsg){
	printf("Error: %s\n", errmsg);
}

//Function that searches a string array for a string, used to search argv[] array.
int strArraySearch(char const *array[], int len, char *delim){
	for(int i = 0; i < len; ++i){
	    if(!strcmp(array[i], delim))
	        return i;
	}
	return -1;
}