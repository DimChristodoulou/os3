#include "../inc/global.h"

int main(int argc, char *argv[]){

    FILE *configFile = fopen(argv[2], "r");

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
        printf("Retrieved line of length %zu:\n", read);
        word = strtok (line," \n");
        strcpy(configs[i],word);
        while (word != NULL){  
            i++;                    
            word = strtok (NULL, " \n");
            if(word!=NULL)
                strcpy(configs[i],word);
        }
    }

    
    for(i = 0; i < 9; i++){
        if(i%3==0)
            printf(" TYPE OF VESSEL %s\n",configs[i]);
        else if (i%3==1)
            printf(" VESSEL CAPACITY %s\n",configs[i]);
        else if (i%3==2)
            printf(" PAY PER 30MIN %s\n",configs[i]);
    }
    

    //END READ FROM CONFIG FILE

    //ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    //shmget returns an identifier in shmid
    int shmid = shmget(key,1024,0666|IPC_CREAT);
  
    //shmat to attach to shared memory 
    char *str = (char*) shmat(shmid,(void*)0,0);
      
    //Detach from shared memory
    shmdt(str);
    
    //Destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);
}