#include "../inc/global.h"

int main(int argc, char *argv[]){

    //Time seed used to generate random vessels
    srand(time(0));
    
    //Both Semaphores are initialized to zero
    //The portMaster Semaphore shows when the port master should read from the shared memory
    sem_t portMasterSemaphore = createSem("/portMasterSemaphore");

    //The vessel Semaphore shows when the vessel should read from the shared memory
    sem_t vesselSemaphore = createSem("/vesselSemaphore");

    //Shows if the harbor is currently occupied by another vessel.
    sem_t occupiedHarborSemaphore = createSem("/occupiedHarborSemaphore");

    //ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
    //shmget returns an identifier in shmid
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    //shmat to attach to shared memory 
    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
      
    //Create public ledger. Public ledger is a singly linked list of struct publicLedgerRecords
    publicLedger *head = NULL;

    //Copy public ledger to shared memory
    //memcpy(shmemStr, &head, sizeof(int));
    //publicLedgerRecord r1 = createPublicLedger("test",1,2,'S',"testStatus",0,2);
    //push(&head,r1);
    
    /****************************************************** 
     * Format arguments to pass in port master executable *
     ******************************************************/
	char **portMasterArgumentArray = (char**)malloc(10*sizeof(char*));
	for(int i = 0; i < 10; i++)
		portMasterArgumentArray[i] = (char*)malloc(100*sizeof(char));

    strcpy(portMasterArgumentArray[0],"exe/portMaster");
    strcpy(portMasterArgumentArray[1],"-c");
    strcpy(portMasterArgumentArray[2],argv[2]);
    strcpy(portMasterArgumentArray[3],"-s");
    sprintf(portMasterArgumentArray[4],"%d", shmid);
    portMasterArgumentArray[5] = NULL;

    /****************************************************** 
     *   Format arguments to pass in monitor executable   *
     ******************************************************/
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


    /****************************************************** 
     *   Format arguments to pass in vessel executable    *
     ******************************************************/
	char **vesselArgumentArray = (char**)malloc(10*sizeof(char*));
	for(int i = 0; i < 15; i++)
		vesselArgumentArray[i] = (char*)malloc(100*sizeof(char));

    strcpy(vesselArgumentArray[0],"exe/vessel");
    strcpy(vesselArgumentArray[1],"-t");
    strcpy(vesselArgumentArray[3],"-u");
    strcpy(vesselArgumentArray[5],"-p");
    strcpy(vesselArgumentArray[7],"-m");
    strcpy(vesselArgumentArray[9],"-s");
    sprintf(vesselArgumentArray[10],"%d",shmid);
    vesselArgumentArray[11] = NULL;


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
            char quit;
            printf("i to insert\n");
            scanf("%c",&quit);
            while( quit == 'i' ){
                publicLedgerRecord randVessel = createPublicLedger(randstring(10), rand()%5+1, rand()%5+1, randomShipSize(), "new", randOverrideParking(), rand()%3+1);
                
                sprintf(vesselArgumentArray[2],"%c",randVessel.shipSize);
                sprintf(vesselArgumentArray[4],"%d",randVessel.overrideParking);
                sprintf(vesselArgumentArray[6],"%f",randVessel.stayTime);
                sprintf(vesselArgumentArray[8],"%f",randVessel.mantime);

                writeToSharedMem(randVessel, shmemStr);

                pid_t vesselPid = fork();
                if(vesselPid==0){
                    execvp("exe/vessel",vesselArgumentArray);
                }
                printf("i to insert\n");
                scanf(" %c",&quit);
            }
        }
    }
        
    //Detach from shared memory
    shmdt(shmemStr);
    
    //Destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);
}