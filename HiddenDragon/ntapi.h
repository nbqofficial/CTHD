#pragma once
#include <ntifs.h>
#include <windef.h>

NTKERNELAPI
NTSTATUS
PsLookupProcessByProcessId(
	_In_ HANDLE ProcessId,
	_Out_ PEPROCESS* Process
);
