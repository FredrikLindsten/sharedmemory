#pragma once
#include "server.h"
#include <Windows.h>

struct MessageHeader{
	int id, size;
};

class Server{
private:
	bool ownsMemory;
	HANDLE hFileMap;
	void* mData;
	int messageCount, clientCount;

public:
	Server();
	~Server();
	
	void MainLoop();
	bool CreateMessage(void* data, int size);
};