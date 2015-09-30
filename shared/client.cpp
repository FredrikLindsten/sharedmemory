#pragma once
#include <algorithm>
#include <iostream>
#include "mutex.h"
#include "client.h"
#include <thread>


Client::Client(int delay, int memorySize, int numMessages, bool random, int msgSize){
	totalMem = memorySize;
	sleepTime = delay;
	msgsMax = numMessages;
	Mutex* mutex = new Mutex();
	mutex->Lock();
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		memorySize,
		(LPCWSTR) "shared");

	size_t* help;
	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	help = (size_t*)mData;
	mControl = help;
	help += 101;
	mData = (char*)help;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ownsMemory = false;
	else{
		ownsMemory = true;
		std::fill(mControl + 1, mControl + 100, 1);
	}

	bool foundTail=false;
	clientId = 0;
	while (foundTail == false){
		mControl++;
		clientId++;
		if (*mControl == 1){
			foundTail = true;
			*mControl = 0;
			mutex->~Mutex();
		}
	}
	
	MainLoop();
}

bool Client::ReadMessage(){
	char* msg = mData + *mControl;
	int* header = (int*)msg;
	
	int chunks = header[1] / gra;
	if (header[1] % gra != 0)
		chunks++;
	int size = chunks*gra;

	if (*mControl == *(mControl - clientId))
		return false;
	char* buff = new char[size];
	int toEnd = totalMem - *mControl;
	if (size > toEnd){
		memcpy(buff, msg, toEnd);
		memcpy(buff + toEnd, mData, size - toEnd);
	}
	else
		memcpy(buff, mData + *mControl, size);


	*mControl += size;
	if (*mControl > totalMem)
		*mControl -= totalMem;

	msgsRead++;
	std::cout << header[0] << buff+8 << std::endl;
	return true;
}

void Client::MainLoop(){
	bool run = true;
	while(msgsRead<msgsMax){

		run = ReadMessage();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}
}

Client::~Client(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}