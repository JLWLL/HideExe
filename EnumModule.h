#ifndef __MT__NATIVE__API__H__
#define __MT__NATIVE__API__H__

#pragma once

// 这里是为了使用MFC CListBox才包含到
#include "stdafx.h"
extern BOOL StartEnumModuleDeep(HMODULE hModule, CListBox *pListBox);

#pragma warning(disable:4200)

#define REDICT_GET_PROC		GetProcAddress

#ifndef _UNICODE__STRING_
#define _UNICODE__STRING_
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING,*PUNICODE_STRING;
#endif		// end of define _UNICODE__STRING_

typedef struct _ANSI_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} ANSI_STRING, *PANSI_STRING;

typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
}OBJECT_INFORMATION_CLASS,*POBJECT_INFORMATION_CLASS;

typedef struct _OBJECT_BASIC_INFORMATION {

	ULONG                   Attributes;
	ACCESS_MASK             DesiredAccess;
	ULONG                   HandleCount;
	ULONG                   ReferenceCount;
	ULONG                   PagedPoolUsage;
	ULONG                   NonPagedPoolUsage;
	ULONG                   Reserved[3];
	ULONG                   NameInformationLength;
	ULONG                   TypeInformationLength;
	ULONG                   SecurityDescriptorLength;
	LARGE_INTEGER           CreationTime;

} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING          Name;
	WCHAR                   NameBuffer[0];
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG ObjectCount;
	ULONG HandleCount;
	ULONG Reserved1[4];
	ULONG PeakObjectCount;
	ULONG PeakHandleCount;
	ULONG Reserved2[4];
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	UCHAR Unknown;
	BOOLEAN MaintainHandleDatabase;
	PVOID/*POOL_TYPE*/ PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION; 

typedef struct _OBJECT_ALL_TYPES_INFORMATION
{
	ULONG NumberOfTypes;
	OBJECT_TYPE_INFORMATION TypeInformation;

} OBJECT_ALL_TYPES_INFORMATION, *POBJECT_ALL_TYPES_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation, // 0 Y N
	SystemProcessorInformation, // 1 Y N
	SystemPerformanceInformation, // 2 Y N
	SystemTimeOfDayInformation, // 3 Y N
	SystemNotImplemented1, // 4 Y N
	SystemProcessesAndThreadsInformation, // 5 Y N
	SystemCallCounts, // 6 Y N
	SystemConfigurationInformation, // 7 Y N
	SystemProcessorTimes, // 8 Y N
	SystemGlobalFlag, // 9 Y Y
	SystemNotImplemented2, // 10 Y N
	SystemModuleInformation, // 11 Y N
	SystemLockInformation, // 12 Y N
	SystemNotImplemented3, // 13 Y N
	SystemNotImplemented4, // 14 Y N
	SystemNotImplemented5, // 15 Y N
	SystemHandleInformation, // 16 Y N
	SystemObjectInformation, // 17 Y N
	SystemPagefileInformation, // 18 Y N
	SystemInstructionEmulationCounts, // 19 Y N
	SystemInvalidInfoClass1, // 20
	SystemCacheInformation, // 21 Y Y
	SystemPoolTagInformation, // 22 Y N
	SystemProcessorStatistics, // 23 Y N
	SystemDpcInformation, // 24 Y Y
	SystemNotImplemented6, // 25 Y N
	SystemLoadImage, // 26 N Y
	SystemUnloadImage, // 27 N Y
	SystemTimeAdjustment, // 28 Y Y
	SystemNotImplemented7, // 29 Y N
	SystemNotImplemented8, // 30 Y N
	SystemNotImplemented9, // 31 Y N
	SystemCrashDumpInformation, // 32 Y N
	SystemExceptionInformation, // 33 Y N
	SystemCrashDumpStateInformation, // 34 Y Y/N
	SystemKernelDebuggerInformation, // 35 Y N
	SystemContextSwitchInformation, // 36 Y N
	SystemRegistryQuotaInformation, // 37 Y Y
	SystemLoadAndCallImage, // 38 N Y
	SystemPrioritySeparation, // 39 N Y
	SystemNotImplemented10, // 40 Y N
	SystemNotImplemented11, // 41 Y N
	SystemInvalidInfoClass2, // 42
	SystemInvalidInfoClass3, // 43
	SystemTimeZoneInformation, // 44 Y N
	SystemLookasideInformation, // 45 Y N
	SystemSetTimeSlipEvent, // 46 N Y
	SystemCreateSession, // 47 N Y
	SystemDeleteSession, // 48 N Y
	SystemInvalidInfoClass4, // 49
	SystemRangeStartInformation, // 50 Y N
	SystemVerifierInformation, // 51 Y Y
	SystemAddVerifier, // 52 N Y
	SystemSessionProcessesInformation // 53 Y N
}SYSTEM_INFORMATION_CLASS;

typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation,
	MemoryWorkingSetList,
	MemorySectionName,
	MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef LONG NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)


#define ProcessImageFileName 27

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (NTAPI *RTLFREEANSISTRING)(PANSI_STRING);

typedef VOID (NTAPI *RTLINITUNICODESTRING)(PUNICODE_STRING, PCWSTR);

typedef NTSTATUS (NTAPI *RTLUNICODESTRINGTOANSISTRING)(PANSI_STRING, PUNICODE_STRING, BOOLEAN);

typedef NTSTATUS (NTAPI *ZWQUERYSYSTEMINFORMATION)(DWORD, PVOID, DWORD, PDWORD);

typedef NTSTATUS (NTAPI *ZWQUERYINFORMATIONPROCESS)(HANDLE, DWORD,  PVOID, ULONG, PULONG); 

typedef NTSTATUS (NTAPI *RTLADJUSTPRIVILEGE)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);

typedef NTSTATUS (NTAPI *ZWQUERYOBJECT)(HANDLE ObjectHandle,OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, ULONG Length, PULONG ResultLength);

typedef NTSTATUS (NTAPI *ZWQUERYVIRTUALMEMORY)(HANDLE, PVOID, MEMORY_INFORMATION_CLASS, PVOID, ULONG, PULONG);

typedef BOOL (WINAPI *fnGetVolumePathNamesForVolumeNameW)(PWCHAR lpszVolumeName,PWCHAR lpszVolumePathNames,DWORD cchBufferLength,PDWORD lpcchReturnLength);
typedef HANDLE (WINAPI *fnFindFirstVolumeW)(PWCHAR lpszVolumeName, DWORD cchBufferLength);
typedef BOOL (WINAPI *fnFindNextVolumeW)(HANDLE hFindVolume,PWCHAR lpszVolumeName,DWORD cchBufferLength);
typedef BOOL (WINAPI *fnFindVolumeClose)(HANDLE hFindVolume);


extern ZWQUERYINFORMATIONPROCESS			ZwQueryInformationProcess;
extern ZWQUERYSYSTEMINFORMATION				ZwQuerySystemInformation;
extern ZWQUERYOBJECT						ZwQueryObject;
extern ZWQUERYVIRTUALMEMORY					ZwQueryVirtualMemory;

extern RTLUNICODESTRINGTOANSISTRING			RtlUnicodeStringToAnsiString;
extern RTLINITUNICODESTRING					RtlInitUnicodeString;
extern RTLFREEANSISTRING					RtlFreeAnsiString;
extern RTLADJUSTPRIVILEGE					RtlAdjustPrivilege;

int WINAPI	InitlizeNativeAPI();
BOOL WINAPI InitDevice2Path();
BOOL WINAPI StartEnumModuleDeep(CListBox *pListBox);

#endif	// end of define __MT__NATIVE__API__H__