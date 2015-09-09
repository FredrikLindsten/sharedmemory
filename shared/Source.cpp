#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include "server.h"
#include "client.h"


void main(int argc, char* argv[]){

	bool random=false;
	int msgSize=0;
	if (strcmp(argv[5], "random") == 0)
		random = true;
	else
		msgSize = atoi(argv[5]);

	int memSize = atoi(argv[3])*(1 << 20);

	if (strcmp(argv[1],"producer")==0){
		Server* server = new Server(atoi(argv[2]), memSize, atoi(argv[4]), random, msgSize);
	}
	else if (strcmp(argv[1], "consumer") == 0){
		Client* client = new Client(atoi(argv[2]), memSize, atoi(argv[4]), random, msgSize);
	}
	else
		printf("Incorrect arguments");
}