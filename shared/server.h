#pragma once
#include <Windows.h>

struct MessageHeader{
	int id, size;
};

class Server{
private:
	bool ownsMemory;
	HANDLE hFileMap;
	void* mData;
	size_t* mControl;
	int messageCount, clientCount;

public:
	Server();
	~Server();
	
	void MainLoop();
	bool CreateMessage(void* data, int size);
};