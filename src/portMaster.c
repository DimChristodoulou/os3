#include "../inc/portMaster.h"
#include "../inc/global.h"

FILE *fptrWrite;

int main(int argc, char const *argv[])
{
    FILE *configFile;
    int shmid, count;

    time_t rawtime;
  	struct tm * timeinfo;
    char *timeInfoStr;

    fptrWrite = fopen("logfile", "a+");

    //The vessel Semaphore shows when the vessel should read from the shared memory
	sem_t *vesselSemaphore = sem_open("/vesselSemaphore", 0);
	
    //The portMaster Semaphore shows when the port master should read from the shared memory
	sem_t *portMasterSemaphore = sem_open("/portMasterSemaphore", 0);
    
    //Shows if the harbor is currently occupied by another vessel.
	sem_t *occupiedHarborSemaphore = sem_open("/occupiedHarborSemaphore", 1);
    
    //Semaphore that coordinates child processes with parent process.
	sem_t *globalSemaphore = sem_open("/myGlobalSemaphore", 1);

    //Semaphore that coordinates child processes with parent process.
	sem_t *shipLeavingSemaphore = sem_open("/shipLeavingSemaphore", 1);

    int occupiedHarborSemaphoreRetVal, portMasterSemaphoreRetVal, vesselSemaphoreRetVal, globalSemaphoreRetVal, shipLeavingSemaphoreRetVal;

    sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);

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
    int totalCountOfConfigFile = i;
    char typesOfVessels[totalCountOfConfigFile];
    int vesselCapacity[totalCountOfConfigFile], payPer30[totalCountOfConfigFile];
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
    publicLedgerRecord recordToBeInserted;

    char buf[1000];
    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
    char **token;
    token = (char**)malloc(15*sizeof(char*));

    float recordArrivalTime, recordStayingTime;
    
    for(i = 0; i < 15; i++){
        token[i] = (char*)malloc(100*sizeof(char));
    }

    count = 0;

    while(globalSemaphoreRetVal == 0){

        //Wait until a ship shows up
        sem_wait(vesselSemaphore);
        sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
        //printf("SEMAPHORE %d\n", occupiedHarborSemaphoreRetVal);
        sem_getvalue(shipLeavingSemaphore , &shipLeavingSemaphoreRetVal);
        printf("SEMAPHORE LEAVING %d\n", shipLeavingSemaphoreRetVal);
        if( occupiedHarborSemaphoreRetVal > 0 && shipLeavingSemaphoreRetVal > 0){
            count = 0;

            //Signal all ships that the harbor is occupied
            sem_wait(occupiedHarborSemaphore);
            
            sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
            printf("SEMAPHORE %d\n", occupiedHarborSemaphoreRetVal);
            
            //PART THAT INSERTS THE SHIP IN THE PUBLIC LEDGER
            //memcpy(buf, shmemStr, sizeof(publicLedgerRecord));
            readFromSharedMem(buf, shmemStr);
            printf("PORTMASTER BUF IS %s\n",buf);

            time ( &rawtime ); 
			timeinfo = localtime ( &rawtime );
			timeInfoStr = asctime(timeinfo);
			timeInfoStr[strlen(timeInfoStr) - 1] = 0;
			fprintf(fptrWrite," %s : THE PORT MASTER INSERTED THE VESSEL %s IN THE PUBLIC LEDGER [PORT MASTER]\n", timeInfoStr);
            
            token[count++] = strtok(buf, "-");
            do{
                printf("token: \"%s\"\n", token[count-1]);
            }
            while (token[count++] = strtok(NULL, "-"));

            //REMOVE 1 PARKING SPOT ACCORDING TO THE SHIP'S SIZE
            for(i=0;i<totalCountOfConfigFile;i++){
                if(typesOfVessels[i] == token[3][0]){
                    vesselCapacity[i]--;
                }
            }

            recordToBeInserted = createPublicLedger(token[0],atof(token[1]),atof(token[2]),token[3][0],token[4],atoi(token[5]),atof(token[6]));
            push(&head,recordToBeInserted);
            //shmemStr = "";
            //printPublicLedger(recordToBeInserted);
        }
        sem_post(portMasterSemaphore);
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
    }

    //memcpy(&head, shmemStr, sizeof(int));
    //printf("copied %d\n", &head);
    //publicLedgerRecord r1 = pop(&head);
    //printf("CHILD READ %s %f %f %c %c %s\n",r1.shipName, r1.arrivalTime, r1.stayTime, r1.parkingSpace, r1.shipSize, r1.status);
    return 0;
}
