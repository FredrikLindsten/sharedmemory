#pragma once
#include "server.h"
#include <iostream>
#include <Windows.h>
#include <thread>

int gra = 256;

Server::Server(int delay, int memorySize, int numMessages, bool random, int msgSizeIn){
	totalMem = memorySize;
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		memorySize,
		(LPCWSTR) "shared");
	
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
		std::cout << s[i];
	}
	s[len] = 0;
}

int Server::FreeMemory(){
	int free = 0;
	int lastTail = mControl[0];

	size_t* help = new size_t[clientCount];
	memcpy(help, mControl, (clientCount+1)*sizeof(size_t));

	for (int i = 1; i < clientCount; i++){
		if (mControl[i]<mControl[0])
			help[i] += totalMem;
	}

	for (int i = 1; i < clientCount; i++){
		if (help[i] < lastTail||i==1)
			lastTail = help[i];
	}
	help[0] = totalMem;
	free = lastTail - help[0];
	

	return free;
}

bool Server::WriteMessage(char* data, int size){
	int chunks = size / gra;
	if (size % gra != 0)
		chunks++;
	size = chunks*gra;
	if (size > FreeMemory())
		return false;

	memcpy(mData, data, size);
	*mControl;


	return true;
}

void Server::MainLoop(){
	bool run = true;
	int len;
	while (msgCount<msgMax){

		//createdata
		if (msgRandSize = true)
			len = (rand() % msgSize);
		else
			len = msgSize;
		char *buff = new char[len];
		gen_random((char*)buff, len);

		run = WriteMessage(buff, len);
		if (run = true){
			msgCount++;
			if (sleepTime > 1)
				sleepTime--;
		}
		else{
			sleepTime++;
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}
	}

	return;
}


Server::~Server(){
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}