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

publicLedgerRecord createPublicLedger(char name[50], float stayTime, float arrivalTime, char parkingSpace, char shipSize, char status[50]){
	publicLedgerRecord newRecord;
	strcpy(newRecord.shipName,name);
	newRecord.stayTime = stayTime;
	newRecord.arrivalTime = arrivalTime;
	newRecord.parkingSpace = parkingSpace;
	newRecord.shipSize = shipSize;
	strcpy(newRecord.status,status);
	return newRecord;
}

publicLedger* newNode(publicLedgerRecord data) { 
    publicLedger* stackNode = (publicLedger*) malloc(sizeof(publicLedger)); 
    stackNode->data = data; 
    stackNode->next = NULL; 
    return stackNode; 
} 

int isEmpty(publicLedger *root) { 
    return !root; 
} 

void push(publicLedger** root, publicLedgerRecord data) { 
    publicLedger* stackNode = newNode(data); 
    stackNode->next = *root; 
    *root = stackNode; 
    //printf("%d pushed to stack\n", data); 
} 

publicLedgerRecord pop(publicLedger** root) { 
    if (isEmpty(*root)) 
        exit(1);

    publicLedger *temp = *root; 
    *root = (*root)->next; 
    publicLedgerRecord popped = temp->data; 
    free(temp); 
  
    return popped; 
} 