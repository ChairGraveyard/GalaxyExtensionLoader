// SWGEmuSettingsOverrider.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DebugLogging.h"

#include "Memory.h"

#include <windows.h>
#include <shlwapi.h> 
#include <conio.h>
#include <stdlib.h>


using namespace std;


void main()
{
	DEBUG_LOG("Waiting for SWGEmu.exe...." << endl);

	HWND hWnd = FindWindow(0, "SwgClient");
	HANDLE hProcess = NULL;
	DWORD proc_id;

	int counter = 0;
	bool exit = false;

	while (!exit)
	{
		counter++;

		system("cls");

		// Pressed escape? Quit.
		if (GetAsyncKeyState(VK_ESCAPE))
			exit = true;

		DEBUG_LOG("Finding SWG Window....");

		// Try to find SWG window.
		hWnd = FindWindow(0, "SwgClient");

		if (counter > 1)
			Sleep(500);

		if (hWnd == NULL)
			continue;

		DEBUG_LOG("SWG Found.");

		// Get the process ID.
		GetWindowThreadProcessId(hWnd, &proc_id);

		char buf[MAX_PATH] = { 0 };
		GetFullPathName("SWGExtensions.dll", MAX_PATH, buf, NULL);

		// Inject our main dll 
		if (!Inject(proc_id, buf))
		{
			DEBUG_LOG("DLL Not Loaded!");
		}
		else
		{
			DEBUG_LOG("DLL Loaded!");
		}

		break;
	}

	// Cleanup our handle.
	CloseHandle(hProcess);
}

