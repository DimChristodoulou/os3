#include "../inc/portMaster.h"
#include "../inc/global.h"

FILE *fptrWrite;

int main(int argc, char const *argv[])
{

    //ftok to generate unique key 
    key_t key = ftok("shmfile1",65); 
    //shmget returns an identifier in shmid
    int shmid1 = shmget(key,1024,0666|IPC_CREAT);
    //shmat to attach to shared memory 
    char *shmemStr1 = (char*) shmat(shmid1,(void*)0,0);

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

    float payment, totalPayment = 0;

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

    char temp[1000];
    char tempSize;

    while(globalSemaphoreRetVal == 0){        

        //Wait until a ship shows up
        sem_wait(vesselSemaphore);
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
        if(globalSemaphoreRetVal != 0){
            break;
        }
        
        sem_getvalue(vesselSemaphore , &vesselSemaphoreRetVal);
        sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
        sem_getvalue(shipLeavingSemaphore , &shipLeavingSemaphoreRetVal);
        
        //IF A SHIP IS LEAVING
        if(shipLeavingSemaphoreRetVal == 0){
            fprintf(fptrWrite," %s : THE VESSEL %s LEFT THE PORT [PORT MASTER]\n", timeInfoStr, buf);
            //ADD 1 PARKING SPOT ACCORDING TO THE SHIP'S SIZE
            tempSize = getRecordSize(head, buf);
            changeStatus(head,buf,"left");
            fprintf(fptrWrite," %s : CHANGED STATUS OF VESSEL %s FROM PARKED TO LEFT [PORT MASTER]\n", timeInfoStr, buf);
            for(i=0;i<totalCountOfConfigFile;i++){
                if(typesOfVessels[i] == tempSize){
                    vesselCapacity[i]++;                                        
                }
            }
            //ALERT MONITOR PROCESS THAT A CHANGE WAS MADE
            sprintf(temp,"%d %d %d %f", vesselCapacity[0],vesselCapacity[1],vesselCapacity[2], totalPayment);
            memcpy(shmemStr1, temp, sizeof(temp));
            sem_post(portMasterSemaphore);
        }
        
        if( occupiedHarborSemaphoreRetVal > 0 && shipLeavingSemaphoreRetVal > 0){
            count = 0;
            //Signal all ships that the harbor is occupied
            sem_wait(occupiedHarborSemaphore);
            
            sem_getvalue(occupiedHarborSemaphore , &occupiedHarborSemaphoreRetVal);
            
            //PART THAT INSERTS THE SHIP IN THE PUBLIC LEDGER
            readFromSharedMem(buf, shmemStr);

            //GET TIME FOR LOGFILE
            time (&rawtime);
			timeinfo = localtime ( &rawtime );
			timeInfoStr = asctime(timeinfo);
			timeInfoStr[strlen(timeInfoStr) - 1] = 0;
			fprintf(fptrWrite," %s : THE PORT MASTER INSERTED THE VESSEL %s IN THE PUBLIC LEDGER [PORT MASTER]\n", timeInfoStr, buf);
            
            token[count++] = strtok(buf, "-");
            do{}
            while (token[count++] = strtok(NULL, "-"));

            //REMOVE 1 PARKING SPOT ACCORDING TO THE SHIP'S SIZE AND PAY THE PORT MASTER
            for(i=0;i<totalCountOfConfigFile;i++){
                //IF THERE ARE ENOUGH PARKING SLOTS OF SAME SIZE
                if(typesOfVessels[i] == token[3][0] && vesselCapacity[i] > 0){
                    vesselCapacity[i]--;                    
                    payment = ( (float)payPer30[i]/1800)*atof(token[1]);
                    totalPayment += payment;
                    changeStatus(head,buf,"parked");
                    fprintf(fptrWrite," %s : CHANGED STATUS OF VESSEL %s FROM WAITING TO PARKED [PORT MASTER]\n", timeInfoStr, buf);
                }
                //IF THERE ARENT ENOUGH SLOTS OF SAME SIZE
                else if( atoi(token[5]) == 1 ){
                    if(typesOfVessels[0] == token[3][0] && vesselCapacity[0] <= 0 && vesselCapacity[1] > 0){
                        vesselCapacity[1]--;                    
                        payment = ( (float)payPer30[1]/1800)*atof(token[1]);
                        totalPayment += payment;
                        changeStatus(head,buf,"parked");
                        fprintf(fptrWrite," %s : SMALL VESSEL %s PARKED IN MEDIUM SLOT [PORT MASTER]\n", timeInfoStr, buf);
                        fprintf(fptrWrite," %s : CHANGED STATUS OF VESSEL %s FROM WAITING TO PARKED [PORT MASTER]\n", timeInfoStr, buf);
                    }
                    else if(typesOfVessels[0] == token[3][0] && vesselCapacity[1] <= 0 && vesselCapacity[2] > 0){
                        vesselCapacity[2]--;                    
                        payment = ( (float)payPer30[2]/1800)*atof(token[1]);
                        totalPayment += payment;
                        changeStatus(head,buf,"parked");
                        fprintf(fptrWrite," %s : SMALL VESSEL %s PARKED IN LARGE SLOT [PORT MASTER]\n", timeInfoStr, buf);
                        fprintf(fptrWrite," %s : CHANGED STATUS OF VESSEL %s FROM WAITING TO PARKED [PORT MASTER]\n", timeInfoStr, buf);
                    }
                    else if(typesOfVessels[1] == token[3][0] && vesselCapacity[1] <= 0 && vesselCapacity[2] > 0){
                        vesselCapacity[2]--;                    
                        payment = ( (float)payPer30[2]/1800)*atof(token[1]);
                        totalPayment += payment;
                        changeStatus(head,buf,"parked");
                        fprintf(fptrWrite," %s : MEDIUM VESSEL %s PARKED IN LARGE SLOT [PORT MASTER]\n", timeInfoStr, buf);
                        fprintf(fptrWrite," %s : CHANGED STATUS OF VESSEL %s FROM WAITING TO PARKED [PORT MASTER]\n", timeInfoStr, buf);
                    }
                    else if(vesselCapacity[1] <= 0 && vesselCapacity[2] <= 0 && vesselCapacity[3]<=0){
                        fprintf(fptrWrite," %s : NO SPACE FOR VESSEL %s... WAITING [PORT MASTER]\n", timeInfoStr, buf);
                    }
                }
            }            

            //ALERT MONITOR PROCESS THAT A CHANGE WAS MADE
            sprintf(temp,"%d %d %d %f", vesselCapacity[0],vesselCapacity[1],vesselCapacity[2], totalPayment);
            memcpy(shmemStr1, temp, sizeof(temp));
            sem_post(portMasterSemaphore);

            //INSERT SHIP TO LEDGER
            recordToBeInserted = createPublicLedger(token[0],atof(token[1]),atof(token[2]),token[3][0],token[4],atoi(token[5]),atof(token[6]));
            setPayment(recordToBeInserted, payment);

            fprintf(fptrWrite," %s : THE VESSEL %s PAYED %f [PORT MASTER]\n", timeInfoStr, buf, payment);

            push(&head,recordToBeInserted);
            
        }        
        //UPDATE GLOBAL SEMAPHORE TO CHECK THAT PARENT HASN'T EXITED YET
        sem_getvalue(globalSemaphore , &globalSemaphoreRetVal);
    }
    //FINAL PRINTS
    printPublicLedger(head);
    printf("TOTAL PAYMENT: %f", totalPayment);


    //DETACHING SHARED MEMORY
    shmdt(shmemStr);
    shmdt(shmemStr1);
    //Destroy the shared memory
    shmctl(shmid1,IPC_RMID,NULL);

    return 0;
}
