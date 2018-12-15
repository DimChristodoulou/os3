#ifndef GLOBAL__H
#define GLOBAL__H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    #include <sys/sem.h>
    #include <semaphore.h>
    #include <time.h>

    typedef struct publicLedgerRecords{
        char shipName[50];
        float stayTime, arrivalTime;
        char parkingSpace, shipSize;
        char status[50];
        int overrideParking;
    } publicLedgerRecord;

    typedef struct list{
        publicLedgerRecord data;
        struct list *next;
    } publicLedger;

    int strArraySearch(char const *array[], int len, char *delim);
    void errCatch(char* errmsg);
    publicLedgerRecord pop(publicLedger** root);
    publicLedgerRecord createPublicLedger(char name[50], float stayTime, float arrivalTime, char shipSize, char status[50], int overrideParking);
    publicLedger* newNode(publicLedgerRecord data);
    int isEmpty(publicLedger *root);
    void push(publicLedger** root, publicLedgerRecord data);
    sem_t createSem();
    char *randstring(size_t length);
    char randomShipSize();
    int randOverrideParking();

#endif