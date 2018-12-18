#include "../inc/global.h"
#include "../inc/monitor.h"

int main(int argc, char const *argv[])
{

    

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
    int shmid, count;
    char buf[1000];

    int monitortime, stattimes;

    sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);

    if(argc == 7){

		count = strArraySearch(argv, argc, "-d");
		monitortime = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-t");
		stattimes = atoi(argv[++count]);

        count = strArraySearch(argv, argc, "-s");
		shmid = atoi(argv[++count]);
		
	}

    char *shmemStr = (char*) shmat(shmid,(void*)0,0);

    printf("MONITOR %d %d %d \n", monitortime, stattimes, shmid);

    while(globalSemaphoreRetVal == 0){
        //printf("%s SHMEMEM\n",shmemStr);
        sleep(stattimes);        
        memcpy(buf, shmemStr, sizeof(publicLedgerRecord));
        printf("MONITOR BUF IS %s\n",buf);
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);

        sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
        printf("semvalue occupiedHarbor %d\n", occupiedHarborSemaphoreRetVal);
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
        printf("semvalue global  %d\n", globalSemaphoreRetVal);
        sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);
        printf("semvalue vessel  %d\n", vesselSemaphoreRetVal);
        sem_getvalue(portMasterSemaphore , &portMasterSemaphoreRetVal);
        printf("semvalue portMaster  %d\n", portMasterSemaphoreRetVal);
        sem_getvalue(shipLeavingSemaphore , &shipLeavingSemaphoreRetVal);
        printf("semvalue shipLeaving  %d\n", shipLeavingSemaphoreRetVal);
    }

    return 0;
}
