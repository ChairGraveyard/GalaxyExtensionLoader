// SWGEmuSettingsOverrider.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DebugLogging.h"

#include "Memory.h"

#include "json.h"

#include <windows.h>
#include <shlwapi.h> 
#include <conio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

using namespace std;
using json = nlohmann::json;

void main()
{
	ifstream configFile;
	
	configFile.open("config.json");

	bool foundConfig = configFile.is_open();

	json config;

	string windowName = "SwgClient";
	string dllName = "SWGCommandExtension.dll";

	if (foundConfig)
	{
		configFile >> config;

		windowName = config["windowName"].get<string>();
		dllName = config["dllName"].get<string>();
	}

	DEBUG_LOG("Waiting for SWGEmu.exe...." << endl);

	HWND hWnd = FindWindow(0, windowName.c_str());
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
		hWnd = FindWindow(0, windowName.c_str());

		if (counter > 1)
			Sleep(500);

		if (hWnd == NULL)
			continue;

		DEBUG_LOG("SWG Found.");

		// Get the process ID.
		GetWindowThreadProcessId(hWnd, &proc_id);

		char buf[MAX_PATH] = { 0 };
		GetFullPathName(dllName.c_str(), MAX_PATH, buf, NULL);

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

