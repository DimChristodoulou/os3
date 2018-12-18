#include "../inc/global.h"
#include "../inc/monitor.h"

int main(int argc, char const *argv[])
{

    //ftok to generate unique key 
    key_t key = ftok("shmfile1",65); 
    //shmget returns an identifier in shmid
    int shmid1 = shmget(key,1024,0666);
    //shmat to attach to shared memory 
    char *shmemStr1 = (char*) shmat(shmid1,(void*)0,0);

    //The vessel Semaphore shows when the vessel should read from the shared memory
	sem_t *vesselSemaphore = sem_open("/vesselSemaphore", 0);
	//The portMaster Semaphore shows when the port master should read from the shared memory
	sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", 0);
    //Shows if the harbor is currently occupied by another vessel.
	sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", 1);
    //Semaphore that coordinates child processes with parent process.
	sem_t *globalSemaphore = sem_open("/myGlobalSemaphore", 1);
    sem_t *shipLeavingSemaphore = sem_open("/shipLeavingSemaphore", 1);    

    int occupiedHarborSemaphoreRetVal, portMasterSemaphoreRetVal, vesselSemaphoreRetVal, globalSemaphoreRetVal, shipLeavingSemaphoreRetVal;
    int shmid, count, count1;
    char buf[1000], buf1[1000];
    char **token, **token1;
    token = (char**)malloc(15*sizeof(char*));
    token1 = (char**)malloc(15*sizeof(char*));
    for(int i = 0; i < 15; i++){
        token[i] = (char*)malloc(100*sizeof(char));
        token1[i] = (char*)malloc(100*sizeof(char));
    }

    publicLedgerRecord recordToBeInserted;
    publicLedger *head;

    int monitortime, stattimes;

    sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);

    //ARGUMENT HANDLING
    if(argc == 7){
		count = strArraySearch(argv, argc, "-d");
		monitortime = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-t");
		stattimes = atoi(argv[++count]);

        count = strArraySearch(argv, argc, "-s");
		shmid = atoi(argv[++count]);
	}

    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
    float payment;
    //printf("MONITOR %d %d %d \n", monitortime, stattimes, shmid);

    while(globalSemaphoreRetVal == 0){
        count = 0;
        count1 = 0;
        sleep(stattimes);
        sem_getvalue(portMasterSemaphore, &portMasterSemaphoreRetVal);

        if(portMasterSemaphoreRetVal >= 1){
            memcpy(buf1, shmemStr1, 1000);
            if(buf1 != NULL){                
                token1[count1++] = strtok(buf1, " ");
                do{}
                while (token1[count1++] = strtok(NULL, " "));
                printf("\n*********************************\n");
                printf("| [MONITOR] UPDATE :   \t\t|\n");
                printf("| SMALL PARKING SPOTS LEFT : %s|\n", token1[0]);
                printf("| MEDIUM PARKING SPOTS LEFT : %s|\n", token1[1]);
                printf("| LARGE PARKING SPOTS LEFT : %s |\n", token1[2]);
                printf("| TOTAL PROFIT : %s \t|\n", token1[3]);
                printf("*********************************\n");
            }
            sem_wait(portMasterSemaphore);
        }
        else{
            printf("\n******************************************\n");
            printf("|[MONITOR] : NO CHANGES SINCE LAST UPDATE|\n");
            printf("******************************************\n");
        }

        //UPDATE GLOBAL SEMAPHORE TO CHECK THAT PARENT HASN'T EXITED YET
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
    }

    //DETACHING SHARED MEMORY
    shmdt(shmemStr);
    shmdt(shmemStr1);
    

    return 0;
}
