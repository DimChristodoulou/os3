#include "../inc/vessel.h"
#include "../inc/global.h"

int main(int argc, char const *argv[])
{
    
    char type;
    int postype, parkperiod, mantime, shmid, count;

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

    printf("%c %d %d %d %d\n", type, postype, parkperiod, mantime, shmid);

    return 0;
}
