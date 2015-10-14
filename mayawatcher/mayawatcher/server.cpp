#pragma once
#include "server.h"
#include <iostream>
#include <Windows.h>
#include <thread>


Server::Server(int memorySize){
	totalMem = memorySize*(1 << 20);
	totalMem += sizeof(size_t)*9;
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		totalMem,
		(LPCWSTR) "shared");

	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	size_t* help = (size_t*)mData;
	mControl = help;
	help += 11;
	mData = (char*)help;
	
	*mControl = 0;

	clientCount = 0;
	bool foundClients = false;
	while (foundClients == false){
		clientCount++;
		if (clientCount == 11)
			break;
		if (*(mControl+clientCount) == 1){
			foundClients = true;
			clientCount--;
		}
	}
}

int Server::FreeMemory(){
	int free = 0;
	size_t lastTail = mControl[0];

	size_t* help = new size_t[clientCount];
	memcpy(help, mControl, (clientCount+1)*sizeof(size_t));
	
	for (int i = 0; i < clientCount; i++){
		if (mControl[i+1]<=mControl[0])
			help[i+1] += totalMem;
	}

	for (int i = 0; i < clientCount; i++){
		if (help[i+1] < lastTail || i == 0)
			lastTail = help[i+1];
	}
	free = lastTail - *mControl;
	
	return free;
}

bool Server::WriteMessage(char* data, int size, int type){

	int* header = (int*)data;
	header[0] = msgCount;
	header[1] = size;
	header[2] = type;

	int chunks = size / gra;
	if (size % gra != 0)
		chunks++;
	size = chunks*gra;

	if ((size+gra) > FreeMemory())
		return false;
	int toEnd = totalMem - mControl[0];
	if (size > toEnd){
		memcpy(mData + *mControl, data, toEnd);
		memcpy(mData, data + toEnd, size - toEnd);
	}
	else
		memcpy(mData + *mControl, data, size);
	

	*mControl += size;
	if (*mControl > totalMem)
		*mControl -= totalMem;

	msgCount++;

	return true;
}

Server::~Server(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}