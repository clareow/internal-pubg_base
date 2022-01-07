#pragma once
#include <Windows.h>
#include <sysinfoapi.h>
#pragma warning(disable : 4996)
#define ALIGN_DOWN_BY(length, alignment) \
    ((ULONG_PTR)(length) & ~((ULONG_PTR)(alignment) - 1))

#define ALIGN_DOWN_POINTER_BY(address, alignment) \
    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)(alignment) - 1)))
class MemoryManager {
private:
	SYSTEM_INFO sysInfo{};
	BOOL InfoInit = FALSE;
public:
	void InitialInfo() {
		GetSystemInfo(&sysInfo);
		InfoInit = TRUE;
	}

	BOOL PTRCheck(LPCVOID lp, UINT_PTR ucb)
	{
		if (InfoInit == FALSE)
			InitialInfo();
		ULONG PageSize;
		BOOLEAN Result = FALSE;
		volatile CHAR* Current;
		PCHAR Last;

		if (!ucb) return FALSE;
		if (!lp) return TRUE;
		PageSize = sysInfo.dwPageSize;

		Current = (volatile CHAR*)lp;
		Last = (PCHAR)((ULONG_PTR)lp + ucb - 1);

		if (Last < Current) return TRUE;

		__try
		{
			*Current;

			Current = (volatile CHAR*)ALIGN_DOWN_POINTER_BY(Current, PageSize);
			Last = (PCHAR)ALIGN_DOWN_POINTER_BY(Last, PageSize);

			while (Current != Last)
			{
				Current += PageSize;
				*Current;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			Result = TRUE;
		}
		return Result;
	}

	template <typename T>
	inline T RVM(DWORD_PTR Address) {
		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(T))) {
			return 0;
		}
		return *(T*)Address;
	}
	template <typename T>
	BOOL WVM(DWORD_PTR Address, T Value) {
		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(T))) {
			return 0;
		}
		*(T*)Address = Value;
	}

	inline FTransform ReadFTransform(DWORD_PTR Address) {
		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(FTransform))) {
			return FTransform();
		}
		return *(FTransform*)Address;
	}

	inline Vector3 ReadVector3(DWORD_PTR Address) {
		if (PTRCheck(reinterpret_cast<PVOID>(Address), sizeof(Vector3))) {
			return Vector3(0, 0, 0);
		}
		return *(Vector3*)Address;
	}

	inline BOOL READ_STRING(DWORD_PTR Address, char* buff) {
		if (PTRCheck(reinterpret_cast<PVOID>(Address), 64)) {
			strcpy(buff, "fail");
			return 0;
		}
		char* p = (char*)Address;

		try {
			memcpy(buff, p, 64);
		}
		catch (...)
		{
			strcpy(buff, "fail");
			return 1;
		}
		return 0;
	}
};

static MemoryManager* mem = new MemoryManager;