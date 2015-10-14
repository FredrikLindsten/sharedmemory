#pragma once
#include "server.h"
#include <iostream>
#include <Windows.h>
#include <thread>


Server::Server(int delay, int memorySize, int numMessages, bool random, int msgSizeIn){
	totalMem = memorySize;
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		memorySize,
		(LPCWSTR) "shared");
	sleepTime = delay;
	msgRandSize = random;
	msgSize = msgSizeIn;
	msgMax = numMessages;
	if (random = true)
		msgSize = memorySize / 2;
	size_t* help;
	mData = (char*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	help = (size_t*)mData;
	mControl = help;
	help += 101;
	mData = (char*)help;
	
	*mControl = 0;

	srand(0);
	
	
	clientCount = 0;
	bool foundClients = false;
	while (foundClients == false){
		clientCount++;
		if (*(mControl+clientCount) == 1){
			foundClients = true;
			clientCount--;
		}
	}

	if (clientCount == 0){
		throw std::runtime_error("No clients found!:(");
	}
	MainLoop();
}

void Server::gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	int* header = (int*)s;
	header[0] = msgCount;
	header[1] = len;
	for (auto i = 8; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum)-1)];
	}
	s[len] = 0;
	std::cout << msgCount << s+8 << std::endl;
}

int Server::FreeMemory(){
	int free = 0;
	int lastTail = mControl[0];

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

bool Server::WriteMessage(char* data, int size){
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

void Server::MainLoop(){
	bool run = true;
	int len;
	while (msgCount<msgMax){

		//createdata
		if (msgRandSize == true)
			len = (rand() % (msgSize-8))+8;
		else
			len = msgSize;
		char *buff = new char[len];
		gen_random((char*)buff, len);

		run = WriteMessage(buff, len);

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}
	int stop = 0;
	return;
}


Server::~Server(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}