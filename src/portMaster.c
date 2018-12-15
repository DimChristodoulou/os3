#include "../inc/portMaster.h"
#include "../inc/global.h"

int main(int argc, char const *argv[])
{
    FILE *configFile;
    int shmid, count;

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
    int i=0,j=0;

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
    
    // for(i = 0; i < 9; i++){
    //     if(i%3==0)
    //         printf(" TYPE OF VESSEL %s\n",configs[i]);
    //     else if (i%3==1)
    //         printf(" VESSEL CAPACITY %s\n",configs[i]);
    //     else if (i%3==2)
    //         printf(" PAY PER 30MIN %s\n",configs[i]);
    // }
    //END READ FROM CONFIG FILE

    printf("%d\n", shmid);
    char *shmemStr = (char*) shmat(shmid,(void*)0,0);
    printf("attached\n");
    publicLedger *head = NULL;
    memcpy(&head, shmemStr, sizeof(int));
    //printf("copied %d\n", &head);
    publicLedgerRecord r1 = pop(&head);
    printf("CHILD READ %s %f %f %c %c %s\n",r1.shipName, r1.arrivalTime, r1.stayTime, r1.parkingSpace, r1.shipSize, r1.status);
    return 0;
}
