#ifndef GLOBAL__H
#define GLOBAL__H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>

    int strArraySearch(char const *array[], int len, char *delim);
    void errCatch(char* errmsg);

#endif