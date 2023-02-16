#include "stdafx.h"
#include "EnumModule.h"

ZWQUERYINFORMATIONPROCESS			ZwQueryInformationProcess = NULL;
ZWQUERYSYSTEMINFORMATION			ZwQuerySystemInformation = NULL;
ZWQUERYOBJECT						ZwQueryObject = NULL;
ZWQUERYVIRTUALMEMORY				ZwQueryVirtualMemory = NULL;

RTLUNICODESTRINGTOANSISTRING		RtlUnicodeStringToAnsiString = NULL;
RTLINITUNICODESTRING				RtlInitUnicodeString = NULL;
RTLFREEANSISTRING					RtlFreeAnsiString = NULL;
RTLADJUSTPRIVILEGE					RtlAdjustPrivilege = NULL;

CMap<CString, LPCTSTR, CString, CString>	g_mapDevice2Path;
BOOL										g_bInitPath = FALSE;

// 初始化Native API指针
int WINAPI InitlizeNativeAPI()
{
	HMODULE hNtDll = NULL;

	hNtDll = LoadLibrary(_T("ntdll"));
	if(hNtDll == NULL)
		return FALSE;

	if(ZwQuerySystemInformation == NULL)
		ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)REDICT_GET_PROC(hNtDll,"ZwQuerySystemInformation");

	if(RtlUnicodeStringToAnsiString == NULL)
		RtlUnicodeStringToAnsiString = (RTLUNICODESTRINGTOANSISTRING)REDICT_GET_PROC(hNtDll,"RtlUnicodeStringToAnsiString");

	if(RtlInitUnicodeString == NULL)
		RtlInitUnicodeString = (RTLINITUNICODESTRING)REDICT_GET_PROC(hNtDll,"RtlInitUnicodeString");

	if(RtlFreeAnsiString == NULL)
		RtlFreeAnsiString = (RTLFREEANSISTRING)REDICT_GET_PROC(hNtDll,"RtlFreeAnsiString");

	if(ZwQueryInformationProcess == NULL)
		ZwQueryInformationProcess = (ZWQUERYINFORMATIONPROCESS)REDICT_GET_PROC(hNtDll,"ZwQueryInformationProcess");

	if(RtlAdjustPrivilege == NULL)
		RtlAdjustPrivilege = (RTLADJUSTPRIVILEGE)REDICT_GET_PROC(hNtDll,"RtlAdjustPrivilege");

	if(ZwQueryObject == NULL)
		ZwQueryObject = (ZWQUERYOBJECT)REDICT_GET_PROC(hNtDll,"ZwQueryObject");

	if(ZwQueryVirtualMemory == NULL)
		ZwQueryVirtualMemory=(ZWQUERYVIRTUALMEMORY)REDICT_GET_PROC(hNtDll,"ZwQueryVirtualMemory");

	if(!g_bInitPath)
	{
		InitDevice2Path();
		g_bInitPath = TRUE;
	}

	FreeLibrary(hNtDll);
	return TRUE;
}

// 将文件的物理路径转换为文件路径
void WINAPI ConvertVolumePaths(IN PWCHAR DeviceName, IN PWCHAR VolumeName)
{
	DWORD	CharCount = MAX_PATH + 1;
	PWCHAR	Names     = NULL;
	PWCHAR	NameIdx   = NULL;
	BOOL	Success   = FALSE;

	for (;;)
	{
		//  Allocate a buffer to hold the paths.
		Names = (PWCHAR) new BYTE [CharCount * sizeof(WCHAR)];

		if ( !Names )
		{
			//  If memory can't be allocated, return.
			return;
		}

		//  Obtain all of the paths
		//  for this volume.
		Success = GetVolumePathNamesForVolumeNameW(VolumeName, Names, CharCount, &CharCount);
		if ( Success )
		{
			break;
		}

		if ( GetLastError() != ERROR_MORE_DATA )
		{
			break;
		}

		//  Try again with the
		//  new suggested size.
		delete [] Names;
		Names = NULL;
	}

	if ( Success )
	{
		//  Display the various paths.
		for ( NameIdx = Names;
			NameIdx[0] != L'\0';
			NameIdx += wcslen(NameIdx) + 1 )
		{
			CString		strPath = DeviceName;	// \\Device\\HarddiskVolume1
			CString		strDisk = Names;		// C:\\

			strPath.MakeUpper();		
			strDisk = strDisk.Left(2);// 转为C:
			g_mapDevice2Path.SetAt(strPath, strDisk);
		}
	}

	if ( Names != NULL )
	{
		delete [] Names;
		Names = NULL;
	}
	return;
}

// 遍历磁盘列表，将对应关系存储至CMap
BOOL WINAPI InitDevice2Path()
{
	BOOL   bRet               = FALSE; 
	DWORD  CharCount           = 0;
	WCHAR  DeviceName[MAX_PATH] = L"";
	DWORD  Error              = ERROR_SUCCESS;
	HANDLE FindHandle          = INVALID_HANDLE_VALUE;
	BOOL   Found              = FALSE;
	size_t Index              = 0;
	BOOL   Success                = FALSE;
	WCHAR  VolumeName[MAX_PATH] = L"";

	// 初始化磁盘路径映射表
	g_mapDevice2Path.InitHashTable(256);

	//  Enumerate all volumes in the system.
	FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

	if (FindHandle == INVALID_HANDLE_VALUE)
	{
		Error = GetLastError();
		//wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
		return bRet;
	}

	for (;;)
	{
		//  Skip the \\?\ prefix and remove the trailing backslash.
		Index = wcslen(VolumeName) - 1;

		if (VolumeName[0]     != L'\\' ||
			VolumeName[1]     != L'\\' ||
			VolumeName[2]     != L'?'  ||
			VolumeName[3]     != L'\\' ||
			VolumeName[Index] != L'\\')
		{
			Error = ERROR_BAD_PATHNAME;
			//wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
			break;
		}

		//  QueryDosDeviceW doesn't allow a trailing backslash,
		//  so temporarily remove it.
		VolumeName[Index] = L'\0';
		CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));
		VolumeName[Index] = L'\\';
		if ( CharCount == 0 )
		{
			Error = GetLastError();
			//wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
			break;
		}
		ConvertVolumePaths(DeviceName, VolumeName);

		//  Move on to the next volume.
		Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));
		if ( !Success )
		{
			Error = GetLastError();
			if (Error != ERROR_NO_MORE_FILES)
			{
				//wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
				break;
			}

			//  Finished iterating
			//  through all the volumes.
			Error = ERROR_SUCCESS;
			break;
		}
	}

	FindVolumeClose(FindHandle);
	FindHandle = INVALID_HANDLE_VALUE;

	return bRet;
}

// 将
// “\Device\HarddiskVolume1\Windows\System32\user32.dll”
// 转换为
// “C:\Windows\System32\user32.dll”
void WINAPI DeviceName2PathName(OUT WCHAR* szPathName, IN const WCHAR* szDeviceName)
{
	CString		strPath = _T("");
	CString		strRet, strTemp;
	int			nPos;

	// 取出第三个\\的路径
	strRet.Format(_T("%s"), szDeviceName);
	strRet.MakeUpper();

	nPos = strRet.Find(_T("\\"));
	if(nPos == -1)
		return;

	nPos = strRet.Find(_T("\\"), nPos + 1);
	if(nPos == -1)
		return;

	nPos = strRet.Find(_T("\\"), nPos + 1);
	if(nPos == -1)
		return;

	strTemp = strRet.Left(nPos );
	strTemp.MakeUpper();

	memset(szPathName, 0, MAX_PATH * sizeof(WCHAR));
	if(g_mapDevice2Path.Lookup(strTemp, strPath))
	{
		strRet.Replace(strTemp, strPath);
		wcscpy(szPathName, strRet);
	}
}

/*
typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID  BaseAddress;			//查询内存块所占的第一个页面基地址
	PVOID  AllocationBase;		//内存块所占的第一块区域基地址，小于等于BaseAddress，也就是说BaseAddress一定包含在AllocationBase分配的范围内
	DWORD  AllocationProtect;	//区域被初次保留时赋予的保护属性
	WORD   PartitionId;
	SIZE_T RegionSize;			//从BaseAddress开始，具有相同属性的页面的大小
	DWORD  State;				//内存页面的状态，有三种可能值：MEM_COMMIT、MEM_FREE和MEM_RESERVE
	DWORD  Protect;				//内存页面的属性，其可能的取值与AllocationProtect相同
	DWORD  Type;				//该内存块的类型，有三种可能值：MEM_IMAGE、MEM_MAPPED、MEM_PRIVATE
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
*/

//
// 暴力搜索R3断链隐藏模块
//
BOOL WINAPI StartEnumModuleDeep(CListBox *pListBox)
{
	DWORD		ModuleBase = 0;
	DWORD		ModuleSize = 0;
	WCHAR		szModuleName[MAX_PATH] = {0};
	WCHAR		szPathName[MAX_PATH] = {0};
	HANDLE		hProcess = INVALID_HANDLE_VALUE;
	const ULONG	NameSize = 0x100;
	ULONG		nSize = 0;
	LPVOID		pBuffer = NULL;
	NTSTATUS	status;

	// 清空上次的记录
	if(NULL == pListBox || NULL == pListBox->GetSafeHwnd())
		return FALSE;
	pListBox->ResetContent();

	// 遍历当前进程
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());
	if(!hProcess)
		return FALSE;

	for(int i=-1, BaseAddress=0; BaseAddress<0x80000000; BaseAddress+=0x1000)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if(NT_SUCCESS(ZwQueryVirtualMemory(hProcess, (PVOID)BaseAddress, MemoryBasicInformation, &mbi, sizeof(mbi), 0)))
		{
			if(ModuleBase && ModuleBase != (DWORD)mbi.AllocationBase && ModuleSize) 
				ModuleBase = ModuleSize = 0;

			if(mbi.Type != MEM_IMAGE)
				continue;

			ModuleBase = (DWORD)mbi.AllocationBase;
			if(ModuleBase && mbi.BaseAddress == (PVOID)BaseAddress)
				ModuleSize = (DWORD)mbi.BaseAddress + mbi.RegionSize - (DWORD)mbi.AllocationBase;
			
			if(mbi.AllocationBase != (PVOID)BaseAddress)
				continue;

			if(ModuleBase == 0 || ModuleSize == 0)
				continue;

			pBuffer = malloc(NameSize);
			if(!pBuffer)
			{
				ASSERT(FALSE);
				continue;
			}

			status = ZwQueryVirtualMemory(hProcess, (PVOID)mbi.AllocationBase, MemorySectionName, pBuffer, NameSize, &nSize);
			if(status == STATUS_INFO_LENGTH_MISMATCH || status == STATUS_BUFFER_OVERFLOW)
			{
				free(pBuffer);
				pBuffer = malloc(nSize);
				status = ZwQueryVirtualMemory(hProcess, (PVOID)mbi.AllocationBase, MemorySectionName, pBuffer, nSize, 0);
			}

			if(!pBuffer) continue;
			if(!NT_SUCCESS(status))
			{
				free(pBuffer);
				continue;
			}

			POBJECT_NAME_INFORMATION	SectionName = (POBJECT_NAME_INFORMATION)pBuffer;
			PUNICODE_STRING				usSectionName;

			usSectionName = (PUNICODE_STRING)pBuffer;
			_wcsnicmp(szModuleName, usSectionName->Buffer, usSectionName->Length / sizeof(WCHAR));
			wcsncpy(szModuleName, usSectionName->Buffer, usSectionName->Length / sizeof(WCHAR) );
			szModuleName[usSectionName->Length / sizeof(WCHAR)] = UNICODE_NULL;
			DeviceName2PathName(szPathName, szModuleName);

			// 添加到ListBox
			pListBox->AddString(szPathName);

			// 释放内存
			free(pBuffer);
			pBuffer = NULL;
		} else if(ModuleBase && ModuleSize)
		{
			ModuleBase = ModuleSize = 0;
		}
	}
	CloseHandle(hProcess);
	return FALSE;
}