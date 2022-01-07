#pragma once
#include <windows.h>
#include <MinHook.h>
#include <dxgi.h>
#include <d3d11.h>
#include <cstdint>
#include <string>
#include "defines.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib") 

struct Hooks
{
	static constexpr auto PresentIndex = 8;
	static constexpr auto ResizeBuffersIndex = 13;
};

extern int Width;
extern int Height;
extern uint64_t dxgiaddr;
extern FCameraCacheEntry GlobalCamera;
BOOL BitBltHook(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
void HookD3D11();
std::string divisionesp(std::string ActorName_);
std::string divisionitem(std::string ItemName);
AimInfo GetBestTarget();