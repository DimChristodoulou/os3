#include "../inc/global.h"

int main(int argc, char *argv[]){

    //Time seed used to generate random vessels
    srand(time(0));

    sem_t sem1 = createSem();
    sem_t sem2 = createSem();

    //ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
    //shmget returns an identifier in shmid
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    //shmat to attach to shared memory 
    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
      
    //Create public ledger. Public ledger is a singly linked list of struct publicLedgerRecords
    publicLedger *head = NULL;

    //Copy public ledger to shared memory
    memcpy(shmemStr, &head, sizeof(int));
    publicLedgerRecord r1 = createPublicLedger("test",1,2,'S','T',"testStatus");
    push(&head,r1);

    
    //Format arguments to pass in port master executable
	char **portMasterArgumentArray = (char**)malloc(10*sizeof(char*));
	for(int i = 0; i < 10; i++)
		portMasterArgumentArray[i] = (char*)malloc(100*sizeof(char));

    strcpy(portMasterArgumentArray[0],"exe/portMaster");
    strcpy(portMasterArgumentArray[1],"-c");
    strcpy(portMasterArgumentArray[2],argv[2]);
    strcpy(portMasterArgumentArray[3],"-s");
    sprintf(portMasterArgumentArray[4],"%d", shmid);
    portMasterArgumentArray[5] = NULL;

    //Format arguments to pass in monitor executable
	char **monitorArgumentArray = (char**)malloc(10*sizeof(char*));
	for(int i = 0; i < 10; i++)
		monitorArgumentArray[i] = (char*)malloc(100*sizeof(char));

    strcpy(monitorArgumentArray[0],"exe/monitor");
    strcpy(monitorArgumentArray[1],"-d");
    sprintf(monitorArgumentArray[2], "%d", 5);
    strcpy(monitorArgumentArray[3],"-t");
    sprintf(monitorArgumentArray[4], "%d", 3);
    strcpy(monitorArgumentArray[5],"-s");
    sprintf(monitorArgumentArray[6],"%d", shmid);
    monitorArgumentArray[7] = NULL;


    pid_t pid = fork();
    if (pid == 0){
        //Spawn a child process that will become the port-master process
        execvp("exe/portMaster",portMasterArgumentArray);
    }        
    else if (pid < 0){
        errCatch("Error in Fork");
    }
    else{
        pid_t pid2 = fork();
        if (pid2 == 0){
            //Spawn a child process that will become the monitor process
            execvp("exe/monitor",monitorArgumentArray);
        }
        else if (pid2 < 0){
            errCatch("Error in Fork");
        }
        else{
            //Vessel part
            publicLedgerRecord randVessel = createPublicLedger(randstring(10), rand()%5+1, rand()%5+1, randomShipSize(), "new", randOverrideParking);
        }
    }
        
    //Detach from shared memory
    shmdt(shmemStr);
    
    //Destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);
}