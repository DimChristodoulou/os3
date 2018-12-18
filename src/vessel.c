#include "../inc/vessel.h"
#include "../inc/global.h"

FILE *fptrWrite;

int main(int argc, char const *argv[])
{
    
    char type;
    int postype, parkperiod, mantime, shmid, count;

	fptrWrite = fopen("logfile", "a+");
	char *timeInfoStr;

	time_t rawtime;
  	struct tm * timeinfo; 	

	//The vessel Semaphore shows when the vessel should read from the shared memory
	sem_t *vesselSemaphore = sem_open("/vesselSemaphore", 0);

	//The portMaster Semaphore shows when the port master should read from the shared memory
	sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", 0);

    //Shows if the harbor is currently occupied by another vessel.
	sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", 1);

	//Semaphore that coordinates child processes with parent process.
	sem_t *shipLeavingSemaphore = sem_open("/shipLeavingSemaphore", 1);

	//Semaphore that coordinates child processes with parent process.
	sem_t *globalSemaphore = sem_open("/myGlobalSemaphore", 1);

	int occupiedHarborSemaphoreRetVal, portMasterSemaphoreRetVal, vesselSemaphoreRetVal, globalSemaphoreRetVal;
	int isLeaving = 0, isEntered = 0;

	sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
	//printf("semvalue %d\n", globalSemaphoreRetVal);

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

    //printf("NEW VESSEL %c %d %d %d %d\n", type, postype, parkperiod, mantime, shmid);
	
	while(!isEntered){
		sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
		
		// if( occupiedHarborSemaphoreRetVal == 0 ){
		// 	//Wait until harbor is not occupied
		// 	sem_wait(occupiedHarborSemaphore);

		// 	//sem_post(occupiedHarborSemaphore);
		// }

		while(occupiedHarborSemaphoreRetVal==0){
			sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
		}
		if( occupiedHarborSemaphoreRetVal > 0 ){
			//Signal the port master that a new vessel is about to enter the harbor
			sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);
        	//printf("SEMAPHORE VESSEL BEFORE %d\n", vesselSemaphoreRetVal);
			
			sem_post(vesselSemaphore);
			
			sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);
        	//printf("SEMAPHORE VESSEL AFTER %d\n", vesselSemaphoreRetVal);
			
			printf("SHIP ENTERED HARBOR\n");

			time ( &rawtime ); 
			timeinfo = localtime ( &rawtime );
			timeInfoStr = asctime(timeinfo);
			timeInfoStr[strlen(timeInfoStr) - 1] = 0;
			fprintf(fptrWrite," %s : A SHIP ENTERED THE HARBOR [VESSEL]\n", timeInfoStr);

			isEntered = 1;

			//Signal the port master that the vessel is in the harbor and no new vessels should enter
			//sem_wait(occupiedHarborSemaphore);

			//Start "maneuvering" for mantime seconds
			sleep(mantime);
			printf("SHIP PARKED\n");

			time ( &rawtime ); 
			timeinfo = localtime ( &rawtime );
			timeInfoStr = asctime(timeinfo);
			timeInfoStr[strlen(timeInfoStr) - 1] = 0;
			fprintf(fptrWrite," %s : A SHIP IS NOW PARKED [VESSEL]\n", timeInfoStr);

			//After ship is parked, increase harbor semaphore to show that it's not occupied anymore
			sem_post(occupiedHarborSemaphore);
			
			//Sleep for parkperiod seconds
			sleep(parkperiod);
			

			while(!isLeaving){
				sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
				while(occupiedHarborSemaphoreRetVal==0){
					sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
				}
				if(occupiedHarborSemaphoreRetVal>0){
					sem_post(vesselSemaphore);
					sem_wait(shipLeavingSemaphore);
					printf("SHIP LEAVING\n");
					isLeaving = 1;
					sleep(mantime);
					printf("SHIP LEFT\n");
					sem_post(shipLeavingSemaphore);
				}
			}

			
			
		}

	}

    return 0;
}
