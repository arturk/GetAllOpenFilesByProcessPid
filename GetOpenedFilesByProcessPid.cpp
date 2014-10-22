#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <FileAPI.h>
#include <WinBase.h>
#include <conio.h>
#include <ctype.h>

#define START_ALLOC                 0x1000
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004
#define SystemHandleInformation     0x10

typedef long(__stdcall *NtQSI)(
	ULONG  SystemInformationClass,
	PVOID  SystemInformation,
	ULONG  SystemInformationLength,
	PULONG ReturnLength
	);

typedef struct _SYSTEM_HANDLE_ENTRY {
	ULONG  OwnerPid;
	BYTE   ObjectType;
	BYTE   HandleFlags;
	USHORT HandleValue;
	PVOID  ObjectPointer;
	ACCESS_MASK  AccessMask;
} SYSTEM_HANDLE_ENTRY, *PSYSTEM_HANDLE_ENTRY;

int main(int argc, char *argv[])
{
	HMODULE hNtDll = NULL;
	NtQSI   pNtQSI = NULL;
	PVOID   pMem = NULL;
	ULONG   allocSize = START_ALLOC;
	ULONG   retVal = 0;
	// --------------------------------
	ULONG   hCount = 0;
	PSYSTEM_HANDLE_ENTRY hFirstEntry = NULL;
	// --------------------------------
	ULONG   i;

	hNtDll = LoadLibraryA("NTDLL.dll");

	if (!hNtDll)
		return 1;

	pNtQSI = (NtQSI)GetProcAddress(hNtDll, "NtQuerySystemInformation");

	if (!pNtQSI) {
		FreeLibrary(hNtDll);
		return 2;
	}
	
	pMem = malloc(allocSize);
	
	while (pNtQSI(SystemHandleInformation, pMem, allocSize, &retVal)
		== STATUS_INFO_LENGTH_MISMATCH) {
		pMem = realloc(pMem, allocSize *= 2);
	}

	hCount = *(ULONG*)pMem;
	hFirstEntry = (PSYSTEM_HANDLE_ENTRY)((PBYTE)pMem + 4);

	int pid = atoi(argv[1]);

	for (i = 0; i < hCount; ++i)
	if ((hFirstEntry[i].ObjectType == 30) && (hFirstEntry[i].OwnerPid == pid))
	{
		HANDLE TargetHandleValueTemp = (HANDLE)hFirstEntry[i].HandleValue;
		HANDLE SourceProcHandleTemp = OpenProcess(PROCESS_DUP_HANDLE, FALSE, hFirstEntry[i].OwnerPid);
		char confirm ='n';

		DuplicateHandle(SourceProcHandleTemp, (HANDLE)hFirstEntry[i].HandleValue, GetCurrentProcess(), &TargetHandleValueTemp, 0, FALSE, DUPLICATE_SAME_ACCESS);
		
		TCHAR Path[MAX_PATH];
		DWORD dwret = GetFinalPathNameByHandle(TargetHandleValueTemp, Path, MAX_PATH, 0);
		if (_tcsstr(Path, _T(argv[2])))
		{
			_tprintf(TEXT("PID: %d\tFileHandle: %d\tThe final path is: %s\n\tRemove it(y/n)?"), hFirstEntry[i].HandleValue, TargetHandleValueTemp, Path);
			flushall();
			std::cin.get(confirm);
			if (confirm == 'y')
				DuplicateHandle(SourceProcHandleTemp, (HANDLE)hFirstEntry[i].HandleValue, GetCurrentProcess(), &TargetHandleValueTemp, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
		}

		CloseHandle(SourceProcHandleTemp);
		CloseHandle(TargetHandleValueTemp);
	}

	free(pMem);
	FreeLibrary(hNtDll);
}
