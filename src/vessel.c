#include "../inc/vessel.h"
#include "../inc/global.h"

int main(int argc, char const *argv[])
{
    
    char type;
    int postype, parkperiod, mantime, shmid, count;

	//The vessel Semaphore shows when the vessel should read from the shared memory
	sem_t *vesselSemaphore = sem_open("/vesselSemaphore", 0);
	//The portMaster Semaphore shows when the port master should read from the shared memory
	sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", 0);
    //Shows if the harbor is currently occupied by another vessel.
	sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", 1);
	int occupiedHarborSemaphoreRetVal, portMasterSemaphoreRetVal, vesselSemaphoreRetVal;
	int isParked = 0, isEntered = 0;
	sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
	printf("semvalue %d\n", occupiedHarborSemaphoreRetVal);
    //Argument handling
    if(argc == 11){

        count = strArraySearch(argv, argc, "-t");
		type = argv[++count][0];

		count = strArraySearch(argv, argc, "-u");
		postype = atoi(argv[++count]);

		count = strArraySearch(argv, argc, "-p");
		parkperiod = atoi(argv[++count]);

		count = strArraySearch(argv, argc, "-m");
		mantime = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-s");
		shmid = atoi(argv[++count]);
		
	}

    printf("%c %d %d %d %d\n", type, postype, parkperiod, mantime, shmid);

	while(!isEntered){
		sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
		sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);
		printf("semvalue %d\n", occupiedHarborSemaphoreRetVal);
		printf("semvalue vessel %d\n", vesselSemaphoreRetVal);
		if( occupiedHarborSemaphoreRetVal == 0 ){
			printf("not yet\n");
			//Wait until harbor is not occupied
			sem_wait(occupiedHarborSemaphore);
		}
		else{
			printf("in\n");
			//Signal the port master that a new vessel is about to enter the harbor
			sem_post(vesselSemaphore);
			printf("in\n");
			isEntered = 1;
			//Start "maneuvering" for mantime seconds
			sleep(mantime);

		}

	}

    return 0;
}
