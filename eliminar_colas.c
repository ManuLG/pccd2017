#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h> //Prints
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	
	int res = msgctl(atoi(argv[1]), IPC_RMID, 0);
	printf("%d\n", res);
	return 0;
}