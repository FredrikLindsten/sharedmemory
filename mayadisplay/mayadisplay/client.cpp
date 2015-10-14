#pragma once
#include <algorithm>
#include <iostream>
#include "mutex.h"
#include "client.h"
#include <thread>

Client::Client(int memorySize){
	totalMem = memorySize * (1<<20);
	Mutex* mutex = new Mutex();
	mutex->Lock();
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		totalMem,
		(LPCWSTR) "shared");

	size_t* help;
	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	help = (size_t*)mData;
	mControl = help;
	help += 11;
	mData = (char*)help;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ownsMemory = false;
	else{
		ownsMemory = true;
		std::fill(mControl + 1, mControl + 10, 1);
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
}

int Client::ReadMessage(char* buffer){

	if (*mControl == *(mControl - clientId))
		return NoNewMessages;

	char* msg = mData + *mControl;
	int* header = (int*)msg;
	int type = header[2];
	int chunks = header[1] / gra;
	if (header[1] % gra != 0)
		chunks++;
	int size = chunks*gra;

	if (size > (1 << 20))
		return MessageToBig;
	int toEnd = totalMem - *mControl;
	if (size > toEnd){
		memcpy(buffer, msg + 12, toEnd - 12);
		memcpy(buffer + toEnd, mData, size - toEnd);
	}
	else
		memcpy(buffer, mData + (*mControl + 12), size - 12);

	*mControl += size;
	if (*mControl > totalMem)
		*mControl -= totalMem;

	msgsRead++;
	return type;
}

Client::~Client(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}