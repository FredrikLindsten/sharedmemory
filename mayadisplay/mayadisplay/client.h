#pragma once

enum types{ NoNewMessages = -1, MessageToBig = 0, NewMesh = 1, MeshChange = 2, MeshNameChange = 3 };

class Client{
private:
	HANDLE hFileMap;
	char* mData;
	size_t* mControl;
	bool ownsMemory;
	int clientId;
	int totalMem;
	int msgsRead;
	int gra = 256;
public:
	Client(int memorySize);
	~Client();

	int ReadMessage(char* buffer);
};