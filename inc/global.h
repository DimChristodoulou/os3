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
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/wait.h>

    extern FILE *fptrWrite;

    typedef struct publicLedgerRecords{
        char shipName[50];
        float stayTime, arrivalTime;
        char parkingSpace, shipSize;
        char status[50];
        int overrideParking;
        int cost;
        float mantime;
        float payment;
    } publicLedgerRecord;

    typedef struct list{
        publicLedgerRecord data;
        float totalPay;
        struct list *next;
    } publicLedger;

    int strArraySearch(char const *array[], int len, char *delim);
    void errCatch(char* errmsg);
    publicLedgerRecord pop(publicLedger** root);
    publicLedgerRecord createPublicLedger(char name[50], float stayTime, float arrivalTime, char shipSize, char status[50], int overrideParking, float mantime);
    publicLedger* newNode(publicLedgerRecord data);
    int isEmpty(publicLedger *root);
    void push(publicLedger** root, publicLedgerRecord data);
    sem_t createSem(char *name, int initialValue);
    char *randstring(size_t length);
    char randomShipSize();
    int randOverrideParking();
    void printPublicLedgerRecord(publicLedgerRecord rec);
    void writeToSharedMem(publicLedgerRecord rec, char *sharedMem);
    void readFromSharedMem(char *dest, char *sharedMem);
    float setPayment(publicLedgerRecord record, float amount);
    void printPublicLedger( publicLedger *head );
    int searchLedger(publicLedger *head, publicLedgerRecord recordToSearch);
    char getRecordSize(publicLedger *head, char *recordNameToFind);
    void changeStatus(publicLedger *head, char *name, char *status);

#endif