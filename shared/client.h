#pragma once
class Client{
private:
	HANDLE hFileMap;
	char* mData;
	size_t* mControl;
	bool ownsMemory;
	int clientId;
public:
	Client(int delay, int memorySize, int numMessages, bool random, int msgSize);
	~Client();

	void MainLoop();
};