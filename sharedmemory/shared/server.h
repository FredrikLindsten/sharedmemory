#pragma once
#include <Windows.h>

struct MessageHeader{
	int id, size;
};

class Server{
private:
	bool ownsMemory;
	HANDLE hFileMap;
	char* mData;
	size_t* mControl;
	int msgCount, msgMax, clientCount;

	int totalMem;

	int gra = 256;
	bool msgRandSize;
	int msgSize;

	int sleepTime;
public:
	Server(int delay, int memorySize, int numMessages, bool random, int msgSizeIn);
	~Server();
	
	void MainLoop();
	bool WriteMessage(char* data, int size);
	void gen_random(char *s, const int len);
	int FreeMemory();
};