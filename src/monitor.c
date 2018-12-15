#include "../inc/global.h"
#include "../inc/monitor.h"

int main(int argc, char const *argv[])
{

    int monitortime, stattimes, shmid, count;

    if(argc == 7){

		count = strArraySearch(argv, argc, "-d");
		monitortime = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-t");
		stattimes = atoi(argv[++count]);

        count = strArraySearch(argv, argc, "-s");
		shmid = atoi(argv[++count]);
		
	}

    printf("MONITOR %d %d %d \n", monitortime, stattimes, shmid);

    while(1){
        sleep(stattimes);
                
    }

    return 0;
}
