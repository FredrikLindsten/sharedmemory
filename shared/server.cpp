#pragma once
#include "server.h"
#include <Windows.h>

Server::Server(){
	unsigned int mSize = 1 << 10;

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

	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	MainLoop();
}

void Server::MainLoop(){
	bool run = true;

	//createdata
//	messageCount++;
//	void* data;
//	int size;
//
//
//
//	while (run == true){
//		run = CreateMessage(data, size);
//	}

	return;
}

bool Server::CreateMessage(void* data, int size){
	memcpy(mData, data, size);
	return true;
}

Server::~Server(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}