#pragma once
#include <Windows.h>

enum types{ NoNewMessages = -1, MessageToBig, NewMesh, MeshChange, MeshNameChange, MeshMoved };

struct MessageHeader{
	int id, size;
};

class Server{
private:
	bool ownsMemory;
	HANDLE hFileMap;
	char* mData;
	size_t* mControl;
	int msgCount, clientCount;
	int totalMem;
	int gra = 256;
	int FreeMemory();
public:
	Server(int memorySize);
	~Server();
	
	bool WriteMessage(char* data, int size, int type);
};