#include <Windows.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include "D3DHOOK.h"
#include "UGameMemory.h"
#include "MemoryManager.h"
#include "utils.h"
#include "Decrypt.h"
#include "offset.h"
#include "xorstr.h"
#include "UE4Func.h"

int main()
{
	/*if (AllocConsole()) {

		freopen("CONIN$", "rb", stdin);

		freopen("CONOUT$", "wb", stdout);

		freopen("CONOUT$", "wb", stderr);
	}*/

	//printf("Loading...\n");
	UGameData->UpdateAddress();
	UGameData->LastCachedActorPTR = 0x0;
	Util->FindGnames();
	HookD3D11();

	//printf("pLocalController : %IIX\n", UGameData->GetLocalPlayerController());

	/*for (int i = 0x600; i < 0x900; i++)
	{
		float A = mem->RVM<float>(UGameData->GetLocalPlayerController() + i);
		if (A == 2.5f)
			printf("Angle : 0x%X\n", i);
	}*/

	while (1) {
		UGameData->UpdateAddress();
		UGameData->UpdateActors();
		if (GetAsyncKeyState(VK_RBUTTON)) {
			AimInfo a = GetBestTarget();
			UGameData->CachedAimMesh = a.pMesh;
			UGameData->CachedAimActor = a.Actor;
			UGameData->CachedAimRootComp = a.RootComponent;
			UGameData->CachedAimBone = a.BoneState;
		}
		else {
			UGameData->LastCachedActorPTR = 0x0;
			UGameData->CachedAimMesh = 0x0;
			UGameData->CachedAimActor = 0x0;
			UGameData->CachedAimRootComp = 0x0;
		}
		GlobalCamera = UGameData->GetCameraCache();
		Sleep(1);
	}
}

void RemovePeHeader(HMODULE hModule)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader + (DWORD)pDosHeader->e_lfanew);

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}

	if (pNTHeader->FileHeader.SizeOfOptionalHeader)
	{
		DWORD Protect;
		WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
		VirtualProtect((void*)hModule, Size, PAGE_EXECUTE_READWRITE, &Protect);
		SecureZeroMemory((void*)hModule, Size);
		VirtualProtect((void*)hModule, Size, Protect, &Protect);
	}
}

bool __stdcall DllMain(void* module, unsigned long reason, void* buffer)
{
    if (reason == 1) {
        while (!dxgiaddr)
            dxgiaddr = (uint64_t)GetModuleHandle(es("dxgi.dll"));
		RemovePeHeader((HMODULE)module);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(&main), 0, 0, 0);
        return true;
    }
    return false;
}

