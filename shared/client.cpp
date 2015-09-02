#pragma once
#include <algorithm>
#include <iostream>
#include "mutex.h"
#include "client.h"

Client::Client(){
	Mutex* mutex = new Mutex();
	mutex->Lock();
	unsigned int mSize = 1 << 20;
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR) "shared");

	head = (size_t*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(size_t)* 101);
	tail = head;
	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, sizeof(size_t) * 101, 0);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ownsMemory = false;
	else{
		ownsMemory = true;
		std::fill(head, head + 101, 1);
	}

	bool foundTail=false;
	clientId = 0;
	while (foundTail = false){
		tail++;
		clientId++;
		if (*tail == 1){
			foundTail = true;
			*tail = 0;
			mutex->Unlock();
		}
	}
	
	MainLoop();
}

void Client::MainLoop(){
	char* data = "sample data";
	while(1){
		if (*tail!=*head)
			//read
		std::cout << data << std::endl;
	}
}

Client::~Client(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}