#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include "client.h"


void main(int argc, char* argv[]){
	Client* client = new Client(100);
	char* buffer = new char[1 << 20];


	//Main Loop//
	while (1){
		int type = client->ReadMessage(buffer);
		switch (type){
			case NoNewMessages :
				//sleep
				break;
			case NewMesh :
				//createnewmesh
				break;
			case MeshChange :
				//changemesh
				break;
			case MeshNameChange :
				//changemeshname
				break;
		}
	}


}