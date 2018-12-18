#include "../inc/global.h"

FILE *fptrWrite;

int main(int argc, char *argv[]){

    //Time seed used to generate random vessels
    srand(time(0));
    unsigned int val1 = 1, val0 = 0;
    
    fptrWrite = fopen("logfile", "a+");

    //The portMaster Semaphore shows when the port master should read from the shared memory
    sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", O_CREAT, 0644, val0);
    //The vessel Semaphore shows when the vessel should read from the shared memory
    sem_t *vesselSemaphore = sem_open("/vesselSemaphore", O_CREAT, 0644, val0);
    //Shows if the harbor is currently occupied by another vessel.
    sem_t *shipLeavingSemaphore = sem_open("/shipLeavingSemaphore", O_CREAT, 0644, val1);    
    //Semaphore that coordinates child processes with parent process.
    sem_t *globalSemaphore = sem_open("/myGlobalSemaphore", O_CREAT, 0644, val0);
    //Shows if the harbor is currently occupied by another vessel.
    sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", O_CREAT, 0644, val1);    

    int occupiedHarborSemaphoreRetVal,globalSemaphoreRetVal,vesselSemaphoreRetVal, portMasterSemaphoreRetVal, shipLeavingSemaphoreRetVal;

    sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
    sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
    sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);	
    sem_getvalue(portMasterSemaphore , &portMasterSemaphoreRetVal);	
    sem_getvalue(shipLeavingSemaphore , &shipLeavingSemaphoreRetVal);

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
                //Spawn a new vessel with random stats            
                publicLedgerRecord randVessel = createPublicLedger(randstring(10), rand()%10+1, rand()%10+1, randomShipSize(), "waiting", randOverrideParking(), rand()%3+1);
                printPublicLedgerRecord(randVessel);
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

    sem_post(globalSemaphore);
    sem_post(vesselSemaphore);

    int status = 0;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0);
        
    //Detach from shared memory
    shmdt(shmemStr);
    
    //Destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);

    sem_unlink("/portMasterSemaphore");
    sem_unlink("/vesselSemaphore");
    sem_unlink("/occupiedHarborSemaphore");
    sem_unlink("/myGlobalSemaphore");
    sem_unlink("/shipLeavingSemaphore");
    sem_destroy(vesselSemaphore);
    sem_destroy(occupiedHarborSemaphore);
    sem_destroy(portMasterSemaphore);
    sem_destroy(globalSemaphore);
    sem_destroy(shipLeavingSemaphore);

    fclose(fptrWrite);
}