#pragma once
class Client{
private:
	HANDLE hFileMap;
	char* mData;
	size_t* mControl;
	bool ownsMemory;
public:
	Client();
	~Client();

	void Initialize();
};