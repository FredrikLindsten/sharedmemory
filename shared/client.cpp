#pragma once
#include <algorithm>
#include <iostream>
#include "mutex.h"
#include "client.h"

Client::Client(int delay, int memorySize, int numMessages, bool random, int msgSize){
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
		std::fill(mControl, mControl + 101, 1);
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

void Client::MainLoop(){
	int* header = (int*)mData;
	std::cout << header[0] << " " << header[1] << " ";
	while(1){

		for (auto i = 8; i < header[1]; i++)
			std::cout << mData[i];
		std::cout << std::endl;
	}
}

Client::~Client(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}