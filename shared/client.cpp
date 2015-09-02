#pragma once
#include "mutex.h"
#include "client.h"

Client::Client(){
	unsigned int mSize = 1 << 20;

	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR) "shared");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ownsMemory = false;
	else
		ownsMemory = true;

	mControl = (size_t*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(size_t) * 101);
	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, sizeof(size_t) * 101, 0);

	if (ownsMemory){
		for (int i = 0; i < 101; i++){
			*mControl = 1;
			mControl++;
		}
		mControl -= 101;
	}


	bool foundTail=false;
	Mutex* mutex = new Mutex();
	mutex->Lock();
	while (foundTail = false){
		mControl++;
		if (*mControl == 1){
			foundTail = true;
			*mControl = 0;
		}
	}
	mutex->Unlock();
	
}

Client::~Client(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}