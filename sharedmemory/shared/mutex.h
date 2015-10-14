#pragma once
#include <windows.h>
class Mutex {
private:
	HANDLE handle_;
public:
	Mutex(const char* name = "__my_mutex__")
	{
		handle_ = CreateMutex(nullptr, false, (LPCWSTR)name);
	}
	~Mutex()
	{
		ReleaseMutex(handle_);
	}
	void Lock()
	{
		WaitForSingleObject(handle_, INFINITE);
	}
	void Unlock()
	{
		ReleaseMutex(handle_);
	}
};