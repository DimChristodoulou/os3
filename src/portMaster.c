#include "../inc/portMaster.h"
#include "../inc/global.h"

int main(int argc, char const *argv[])
{
    FILE *configFile;
    int shmid, count;

    //The vessel Semaphore shows when the vessel should read from the shared memory
	sem_t *vesselSemaphore = sem_open("/vesselSemaphore", 0);
	//The portMaster Semaphore shows when the port master should read from the shared memory
	sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", 0);
    //Shows if the harbor is currently occupied by another vessel.
	sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", 1);

    int occupiedHarborSemaphoreRetVal, portMasterSemaphoreRetVal, vesselSemaphoreRetVal;

    sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
	printf("semvalue portMASTER %d\n", occupiedHarborSemaphoreRetVal);
    // printf("test\n");
    // sem_wait(&portMasterSemaphore);
    // printf("test\n");
    //printf("%s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);

    if(argc == 5){

		count = strArraySearch(argv, argc, "-s");
		shmid = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-c");
		configFile = fopen(argv[++count], "r");
		
	}    

    //START READ FROM CONFIG FILE
    char *line = NULL;
    char *word;
    size_t len = 0;
    ssize_t read;
    char configs[9][10];
    int i=0;

    if (configFile == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, configFile)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        word = strtok (line," \n");
        strcpy(configs[i],word);
        while (word != NULL){  
            i++;                    
            word = strtok (NULL, " \n");
            if(word!=NULL)
                strcpy(configs[i],word);
        }
    }
    
    char typesOfVessels[i];
    int vesselCapacity[i], payPer30[i];
    int j=0,k=0,m=0;
    for(i = 0; i < 9; i++){
        if(i%3==0){            
            typesOfVessels[j++] = configs[i][0];
        }
        else if (i%3==1){
            vesselCapacity[k++] = atoi(configs[i]);
        }
        else if (i%3==2)
            payPer30[m++] = atoi(configs[i]);
    }
    //END READ FROM CONFIG FILE

    publicLedger *head = NULL;
    char buf[1000];
    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
    
    
    while(1){
        //Wait until a ship shows up
        printf("test\n");
        sem_wait(vesselSemaphore);
        printf("test\n");
        if( occupiedHarborSemaphoreRetVal > 0 ){
            //Signal all ships that the harbor is occupied
            printf("test\n");
            sem_wait(occupiedHarborSemaphore);
            printf("test\n");
            memcpy(buf, shmemStr, sizeof(publicLedgerRecord));
            printf("BUF IS %s\n",buf);
        }
        sem_post(portMasterSemaphore);

    }

    //memcpy(&head, shmemStr, sizeof(int));
    //printf("copied %d\n", &head);
    //publicLedgerRecord r1 = pop(&head);
    //printf("CHILD READ %s %f %f %c %c %s\n",r1.shipName, r1.arrivalTime, r1.stayTime, r1.parkingSpace, r1.shipSize, r1.status);
    return 0;
}
