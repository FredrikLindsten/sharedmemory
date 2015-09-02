#pragma once
class Client{
private:
	HANDLE hFileMap;
	char* mData;
	size_t* tail;
	size_t* head;
	bool ownsMemory;
	int clientId;
public:
	Client();
	~Client();

	void MainLoop();
};