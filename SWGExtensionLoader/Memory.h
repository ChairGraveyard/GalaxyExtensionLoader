#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include "DebugLogging.h"

static void WriteFloat(HANDLE hProc, LPVOID Address, FLOAT Data)
{
	unsigned long protect;
	VirtualProtectEx(hProc, (LPVOID)Address, 4, PAGE_EXECUTE_READWRITE, &protect);
	if (!WriteProcessMemory(hProc, (LPVOID)Address, &Data, sizeof(FLOAT), NULL))
	{
		DEBUG_LOG("Error cannot WriteProcessMemory!");
	}
	VirtualProtectEx(hProc, (LPVOID)Address, 4, protect, &protect);
}

static INT ReadInt(HANDLE hProc, LPVOID Address)
{
	INT readValue = 0;
	ReadProcessMemory(hProc, (LPVOID)Address, &readValue, sizeof(INT), NULL);

	return readValue;
}

static FLOAT ReadFloat(HANDLE hProc, LPVOID Address)
{
	FLOAT readValue = 0.0f;
	ReadProcessMemory(hProc, (LPVOID)Address, &readValue, sizeof(FLOAT), NULL);

	return readValue;
}


static DWORD ReadAddress(HANDLE hProc, LPVOID Address)
{
	DWORD readAddress = 0;
	SIZE_T readBytes = 0;
	ReadProcessMemory(hProc, (LPVOID)Address, &readAddress, sizeof(DWORD), &readBytes);

	if (readBytes <= 0)
		readAddress = 0;

	return readAddress;
}

DWORD GetProcessBaseAddress(DWORD proc_id)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, proc_id);
	MODULEENTRY32 module;
	module.dwSize = sizeof(MODULEENTRY32);
	Module32First(snapshot, &module);

	DWORD baseAddress = 0;

	do
	{
		if (strstr(module.szExePath, "SWGEmu.exe") != NULL)
		{
			baseAddress = (DWORD)module.modBaseAddr;
			break;
		}
	} while (Module32Next(snapshot, &module));

	CloseHandle(snapshot);

	return baseAddress;
}

BOOL Inject(DWORD pId, const char* DllName)
{
	HANDLE Proc;
	//HMODULE hLib;
	char buf[50] = { 0 };
	LPVOID RemoteString, LoadLibAddy;

	if (!pId)
		return false;

	Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
	if (!Proc)
	{
		return false;
	}

	LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	// Allocate space in the process for our DLL 
	RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(DllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// Write the string name of our DLL in the memory allocated 
	WriteProcessMemory(Proc, (LPVOID)RemoteString, DllName, strlen(DllName), NULL);

	// Load our DLL 
	CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);

	CloseHandle(Proc);
	return true;
}