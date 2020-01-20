#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

#define IOCTL_PROCESS_HIDE CTL_CODE(FILE_DEVICE_UNKNOWN, 0xdeadbeef, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define HIDE_STATUS_SUCCESS 808
#define HIDE_STATUS_FAILURE 404

class CrouchingTiger
{
private:
	HANDLE hDriver;

	DWORD GetProcessIdByProcessName(char* processName)
	{
		DWORD pID = NULL;
		HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (ss != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);
			do
			{
				if (!strcmp(pe.szExeFile, processName))
				{
					pID = pe.th32ProcessID;
				}
			} while (Process32Next(ss, &pe));
			CloseHandle(ss);
		}
		return pID;
	}

public:
	CrouchingTiger(char* processName)
	{
		DWORD pid = GetProcessIdByProcessName(processName);
		if (pid != 0)
		{
			printf("[CTHD]: %s found | Process Id: %d\n", processName, pid);
			hDriver = CreateFileA("\\\\.\\HiddenDragon", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
			if (hDriver != INVALID_HANDLE_VALUE)
			{
				printf("[CTHD]: HiddenDragon.sys handle opened\n");

				if (DeviceIoControl(hDriver, IOCTL_PROCESS_HIDE, &pid, sizeof(pid), &pid, sizeof(pid), 0, 0))
				{
					printf("[CTHD]: Hiding %s ...\n", processName);
					if (pid == HIDE_STATUS_SUCCESS)
					{
						printf("[CTHD]: %s successfully hidden\n", processName);
					}
					else
					{
						printf("[CTHD]: %s was not hidden\n", processName);
					}
				}
				else
				{
					printf("[CTHD]: Communication with HiddenDragon.sys failed\n");
				}
				CloseHandle(hDriver);
				printf("[CTHD]: HiddenDragon.sys handle closed\n");
			}
			else
			{
				printf("[CTHD]: HiddenDragon.sys is not loaded\n");
			}
		}
		else
		{
			printf("[CTHD]: %s is not running\n", processName);
		}
		printf("[CTHD]: Exiting\n");
	}
};
