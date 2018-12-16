#include "../inc/global.h"

#define SEMKEYPATH "/tmp"  /* Path used on ftok for semget key  */
#define SEMKEYID 1              /* Id used on ftok for semget key    */
#define SHMKEYPATH "/tmp"  /* Path used on ftok for shmget key  */
#define SHMKEYID 1              /* Id used on ftok for shmget key    */

#define NUMSEMS 2               /* Num of sems in created sem set    */
#define SIZEOFSHMSEG 50         /* Size of the shared mem segment    */

#define NUMMSG 2

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

publicLedgerRecord createPublicLedger(char name[50], float stayTime, float arrivalTime, char shipSize, char status[50], int overrideParking, float mantime){
	publicLedgerRecord newRecord;
	strcpy(newRecord.shipName,name);
	newRecord.stayTime = stayTime;
	newRecord.arrivalTime = arrivalTime;
	newRecord.shipSize = shipSize;
	strcpy(newRecord.status,status);
    newRecord.overrideParking = overrideParking;
    newRecord.cost = 0;
    newRecord.mantime = mantime;
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

void printPublicLedger(publicLedgerRecord rec){
    printf("%s - %fs - %fa - %c - %s - %d - %d\n",rec.shipName, rec.stayTime, rec.arrivalTime, rec.parkingSpace, rec.status, rec.overrideParking, rec.cost);
}

void writeToSharedMem(publicLedgerRecord rec, char *sharedMem){
    char buf[1000];
    sprintf(buf, "%s - %fs - %fa - %c - %s - %d - %d", rec.shipName, rec.stayTime, rec.arrivalTime, rec.parkingSpace, rec.status, rec.overrideParking, rec.cost);
    memcpy(sharedMem, buf, sizeof(publicLedgerRecord));
}

char *randstring(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

char randomShipSize(){
    int sizesArr[] = {76,77,83};
    return sizesArr[rand()%3];
}

int randOverrideParking(){
    return rand()%2;
}

/*************************************************************************
 *                      SHARED MEMORY AND SEMAPHORES                     *
 *************************************************************************/


sem_t createSem(char *name, int initialValue){
    sem_t *sp; 

    /*  Initialize  the  semaphore. */
    sp = sem_open(name, O_CREAT, 0644, initialValue);

    if (sp  == SEM_FAILED) {
        perror("Couldn ’t initialize.");
        exit (3);
    }

    //sem_destroy (&sp);
    return  *sp;
}