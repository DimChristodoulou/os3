#include "../inc/portMaster.h"
#include "../inc/global.h"

int main(int argc, char const *argv[])
{

    FILE *fpCharges;
    int shmid, count;

    if(argc == 5){

		count = strArraySearch(argv, argc, "-c");
		shmid = atoi(argv[++count]);
		
        count = strArraySearch(argv, argc, "-s");
		fpCharges = fopen(argv[++count], "r");
		
	}

    printf("%d\n", shmid);

    return 0;
}
