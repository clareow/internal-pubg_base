#include "D3DHOOK.h"
#include "config.h"
#include "UGameMemory.h"
#include "MemoryManager.h"
#include "Decrypt.h"
#include "offset.h"
#include "utils.h"
#include "xorstr.h"
#include "defines.h"
#include "UE4Func.h"
#include <list>

HWND window = nullptr;
uint64_t dxgiaddr = 0;
IDXGISwapChain* m_pSwapChain;
ID3D11Device* pD3D11_Device = NULL;
ID3D11DeviceContext* pD3D11_Context = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
static HWND g_hWnd = nullptr;
FCameraCacheEntry GlobalCamera{};
static WNDPROC OriginalWndProcHandler = nullptr;
bool g_ShowMenu = true;
LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool isInit = false;
bool BitBitHook = false;

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

static bool is_bitbit_called = true;

typedef long(__stdcall* present_fn) (IDXGISwapChain* p_swapchain, UINT syncintreval, UINT flags);
typedef bool(__stdcall* Bitb)(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);
Bitb o_BitBlt = nullptr;
present_fn o_present = nullptr;

present_fn o_d3dHook = nullptr;
Bitb o_BitBlts = nullptr;

void* m_OriginalPresent{};
void* m_OriginalBitBlt{};

bool __stdcall Hooked_BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) {
	is_bitbit_called = false;
	auto bit = reinterpret_cast<decltype(&Hooked_BitBlt)>(m_OriginalBitBlt)(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
	is_bitbit_called = true;
	return bit;
}

const int MultisampleCount = 1;

int Width{};
int Height{};

#include <iomanip>
#include<conio.h>
#include <sstream>
#include <intrin.h>
#include "imgui.h"

std::string GetProcessorId() {
	std::array<int, 4> cpuInfo;
	__cpuid(cpuInfo.data(), 1);
	std::ostringstream buffer;
	buffer << std::uppercase << std::hex << std::setfill('0')
		<< std::setw(8) << cpuInfo.at(3)
		<< std::setw(8) << cpuInfo.at(0);
	return buffer.str();
}

IDXGISwapChain* GetSwapChain()
{
	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle(es("dxgi.dll"));
		Sleep(1);
	} while (!hDXGIDLL);

	Sleep(1);
	IDXGISwapChain* pSwapChain;
	std::string WindowName = es("LAST");
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL,  WindowName.c_str(), NULL };
	RegisterClassExA(&wc);
	g_hWnd = CreateWindowA(WindowName.c_str(), NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = g_hWnd;
	scd.SampleDesc.Count = MultisampleCount;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(g_hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	UINT createFlags = 0;
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	IDXGISwapChain* d3dSwapChain = 0;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext)))
	{
		return 0;
	}
	else
		return pSwapChain;
}

void DrawStrokeText(const ImVec2& pos, ImU32 col, const char* str, float size = 15.f)
{
	ImGui::GetCurrentWindow()->DrawList->AddText(NULL, size, ImVec2(pos.x, pos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), str);
	ImGui::GetCurrentWindow()->DrawList->AddText(NULL, size, ImVec2(pos.x, pos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), str);
	ImGui::GetCurrentWindow()->DrawList->AddText(NULL, size, ImVec2(pos.x - 1, pos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), str);
	ImGui::GetCurrentWindow()->DrawList->AddText(NULL, size, ImVec2(pos.x + 1, pos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), str);
	ImGui::GetCurrentWindow()->DrawList->AddText(NULL, size, ImVec2(pos.x, pos.y), col, str);
}

void MenuBool(std::string abs, bool flag, int menunum) {
	if(flag)
		DrawStrokeText(ImVec2(Width - 150, Height / 2 - (150 - (menunum * 15))), ImGui::GetColorU32(ImVec4(0, 1, 0, 1)), abs.c_str());
	else
		DrawStrokeText(ImVec2(Width - 150, Height / 2 - (150 - (menunum * 15))), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), abs.c_str());
}


void DrawInfo(const ImVec2& Pos, ImU32 TagCol, int HeightSize, std::string Info, float distance, float EnHealth) {
	ImDrawList* Draw = ImGui::GetCurrentWindow()->DrawList;
	ImVec2 InfoSize = ImGui::CalcTextSize(Info.c_str());
	if (distance < 200) {
		if (EnHealth > 0.0) {
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2), Pos.y + (HeightSize / 2)), ImVec2(Pos.x + (InfoSize.x / 2) + 1, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.7)));
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2), Pos.y + (HeightSize / 2)), ImVec2(Pos.x - (InfoSize.x / 2) + 5, Pos.y - (HeightSize / 2)), TagCol);
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2) + 7, Pos.y + (HeightSize / 2) - 6), ImVec2(Pos.x - (InfoSize.x / 2) + 7 + ((abs((Pos.x - (InfoSize.x / 2) + 7) - (Pos.x + (InfoSize.x / 2))) / 100.0f) * EnHealth), Pos.y + (HeightSize / 2) - 2), ImGui::GetColorU32(ImVec4(0, 1, 0, 1)));
			DrawStrokeText(ImVec2(Pos.x - ((InfoSize.x / 2)) + 10, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), Info.c_str());
		}
		else {
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2), Pos.y + (HeightSize / 2)), ImVec2(Pos.x + (InfoSize.x / 2) + 1, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.7)));
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2), Pos.y + (HeightSize / 2)), ImVec2(Pos.x - (InfoSize.x / 2) + 5, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(0.4, 0.4, 0.4, 1.0f)));
			Draw->AddRectFilled(ImVec2(Pos.x - (InfoSize.x / 2) + 7, Pos.y + (HeightSize / 2) - 6), ImVec2(Pos.x - (InfoSize.x / 2) + 7 + ((abs((Pos.x - (InfoSize.x / 2) + 7) - (Pos.x + (InfoSize.x / 2))) / 100.0f) * EnHealth), Pos.y + (HeightSize / 2) - 2), ImGui::GetColorU32(ImVec4(0, 1, 0, 1)));
			DrawStrokeText(ImVec2(Pos.x - ((InfoSize.x / 2)) + 10, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), Info.c_str());
		}
	}
	else {
		DrawStrokeText(ImVec2(Pos.x - ((InfoSize.x / 2)) + 10, Pos.y - (HeightSize / 2)), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), Info.c_str());
	}
}

int LocalTeamNum = 0;

void DrawStrokeLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness) {
	ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(p1.x - 1, p1.y), ImVec2(p2.x - 1, p2.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), thickness);
	ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), col, thickness);
	ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(p1.x + 1, p1.y), ImVec2(p2.x + 1, p2.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), thickness);
}

void DrawSkeleton(DWORD_PTR pMesh, ImU32 cColor, ImU32 pColor, BoneVisibleResult pBoneVisible) {
	Vector3 vforehead, vneck, vupperarm_l, varm_l, vhand_l, vupperarm_r, varm_r, vhand_r, vspine1, vspine2, vpelvis, vthigh_l, vcalf_l, vfoot_l, vthigh_r, vcalf_r, vfoot_r, vroot;
	Vector3 rforehead, rneck, rupperarm_l, rarm_l, rhand_l, rupperarm_r, rarm_r, rhand_r, rspine1, rspine2, rpelvis, rthigh_l, rcalf_l, rfoot_l, rthigh_r, rcalf_r, rfoot_r, rroot;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	rforehead = Util->GetBoneWithRotation(pMesh, Bones::forehead);
	rneck = Util->GetBoneWithRotation(pMesh, Bones::neck_01);
	rupperarm_l = Util->GetBoneWithRotation(pMesh, Bones::upperarm_l);
	rarm_l = Util->GetBoneWithRotation(pMesh, Bones::lowerarm_l);
	rhand_l = Util->GetBoneWithRotation(pMesh, Bones::hand_l);
	rupperarm_r = Util->GetBoneWithRotation(pMesh, Bones::upperarm_r);
	rarm_r = Util->GetBoneWithRotation(pMesh, Bones::lowerarm_r);
	rhand_r = Util->GetBoneWithRotation(pMesh, Bones::hand_r);
	rspine1 = Util->GetBoneWithRotation(pMesh, Bones::spine_01);
	rspine2 = Util->GetBoneWithRotation(pMesh, Bones::spine_02);
	rpelvis = Util->GetBoneWithRotation(pMesh, Bones::pelvis);
	rthigh_l = Util->GetBoneWithRotation(pMesh, Bones::thigh_l);
	rcalf_l = Util->GetBoneWithRotation(pMesh, Bones::calf_l);
	rfoot_l = Util->GetBoneWithRotation(pMesh, Bones::foot_l);
	rthigh_r = Util->GetBoneWithRotation(pMesh, Bones::thigh_r);
	rcalf_r = Util->GetBoneWithRotation(pMesh, Bones::calf_r);
	rfoot_r = Util->GetBoneWithRotation(pMesh, Bones::foot_r);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vforehead = Util->WorldToScreen(rforehead, UGameData->GetCameraCache());
	vneck = Util->WorldToScreen(rneck, UGameData->GetCameraCache());
	vupperarm_l = Util->WorldToScreen(rupperarm_l, UGameData->GetCameraCache());
	varm_l = Util->WorldToScreen(rarm_l, UGameData->GetCameraCache());
	vhand_l = Util->WorldToScreen(rhand_l, UGameData->GetCameraCache());
	vupperarm_r = Util->WorldToScreen(rupperarm_r, UGameData->GetCameraCache());
	varm_r = Util->WorldToScreen(rarm_r, UGameData->GetCameraCache());
	vhand_r = Util->WorldToScreen(rhand_r, UGameData->GetCameraCache());
	vspine1 = Util->WorldToScreen(rspine1, UGameData->GetCameraCache());
	vspine2 = Util->WorldToScreen(rspine2, UGameData->GetCameraCache());
	vpelvis = Util->WorldToScreen(rpelvis, UGameData->GetCameraCache());
	vthigh_l = Util->WorldToScreen(rthigh_l, UGameData->GetCameraCache());
	vcalf_l = Util->WorldToScreen(rcalf_l, UGameData->GetCameraCache());
	vfoot_l = Util->WorldToScreen(rfoot_l, UGameData->GetCameraCache());
	vthigh_r = Util->WorldToScreen(rthigh_r, UGameData->GetCameraCache());
	vcalf_r = Util->WorldToScreen(rcalf_r, UGameData->GetCameraCache());
	vfoot_r = Util->WorldToScreen(rfoot_r, UGameData->GetCameraCache());
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(pBoneVisible.forehead)
		DrawStrokeLine(ImVec2(vforehead.x, vforehead.y), ImVec2(vneck.x, vneck.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vforehead.x, vforehead.y), ImVec2(vneck.x, vneck.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.upperarm_l)
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vupperarm_l.x, vupperarm_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vupperarm_l.x, vupperarm_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.arm_l)
		DrawStrokeLine(ImVec2(vupperarm_l.x, vupperarm_l.y), ImVec2(varm_l.x, varm_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vupperarm_l.x, vupperarm_l.y), ImVec2(varm_l.x, varm_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.hand_l)
		DrawStrokeLine(ImVec2(varm_l.x, varm_l.y), ImVec2(vhand_l.x, vhand_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(varm_l.x, varm_l.y), ImVec2(vhand_l.x, vhand_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.upperarm_r)
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vupperarm_r.x, vupperarm_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vupperarm_r.x, vupperarm_r.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.arm_r)
		DrawStrokeLine(ImVec2(vupperarm_r.x, vupperarm_r.y), ImVec2(varm_r.x, varm_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vupperarm_r.x, vupperarm_r.y), ImVec2(varm_r.x, varm_r.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.hand_r)
		DrawStrokeLine(ImVec2(varm_r.x, varm_r.y), ImVec2(vhand_r.x, vhand_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(varm_r.x, varm_r.y), ImVec2(vhand_r.x, vhand_r.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.spine2)
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vspine2.x, vspine2.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vneck.x, vneck.y), ImVec2(vspine2.x, vspine2.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.spine1)
		DrawStrokeLine(ImVec2(vspine2.x, vspine2.y), ImVec2(vspine1.x, vspine1.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vspine2.x, vspine2.y), ImVec2(vspine1.x, vspine1.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.pelvis)
		DrawStrokeLine(ImVec2(vspine1.x, vspine1.y), ImVec2(vpelvis.x, vpelvis.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vspine1.x, vspine1.y), ImVec2(vpelvis.x, vpelvis.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.thigh_l)
		DrawStrokeLine(ImVec2(vpelvis.x, vpelvis.y), ImVec2(vthigh_l.x, vthigh_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vpelvis.x, vpelvis.y), ImVec2(vthigh_l.x, vthigh_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.calf_l)
		DrawStrokeLine(ImVec2(vthigh_l.x, vthigh_l.y), ImVec2(vcalf_l.x, vcalf_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vthigh_l.x, vthigh_l.y), ImVec2(vcalf_l.x, vcalf_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.foot_l)
		DrawStrokeLine(ImVec2(vcalf_l.x, vcalf_l.y), ImVec2(vfoot_l.x, vfoot_l.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vcalf_l.x, vcalf_l.y), ImVec2(vfoot_l.x, vfoot_l.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.thigh_r)
		DrawStrokeLine(ImVec2(vpelvis.x, vpelvis.y), ImVec2(vthigh_r.x, vthigh_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vpelvis.x, vpelvis.y), ImVec2(vthigh_r.x, vthigh_r.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.calf_r)
		DrawStrokeLine(ImVec2(vthigh_r.x, vthigh_r.y), ImVec2(vcalf_r.x, vcalf_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vthigh_r.x, vthigh_r.y), ImVec2(vcalf_r.x, vcalf_r.y), pColor, cfg->SkeletonThick);
	if (pBoneVisible.foot_r)
		DrawStrokeLine(ImVec2(vcalf_r.x, vcalf_r.y), ImVec2(vfoot_r.x, vfoot_r.y), cColor, cfg->SkeletonThick);
	else
		DrawStrokeLine(ImVec2(vcalf_r.x, vcalf_r.y), ImVec2(vfoot_r.x, vfoot_r.y), pColor, cfg->SkeletonThick);
}

std::string divisionesp(std::string ActorName_) {
	if (ActorName_ == es("PlayerFemale_A_C") || ActorName_ == es("PlayerMale_A_C") ||
		ActorName_ == es("RegistedPlayer") || ActorName_ == es("AIPawn_Base_C") ||
		ActorName_ == es("AIPawn_Base_Female_C") || ActorName_ == es("AIPawn_Base_Male_C")) {
		return es("Player");
	}
	else if (ActorName_ == es("Carapackage_RedBox_C") || ActorName_ == es("Carapackage_RedBox_FBR_01_C") || ActorName_ == es("Carapackage_FlareGun_C")) {
		return es("AirDrop");
	}
	else if (ActorName_ == es("DeathDropItemPackage_C")) {
		return es("DeathDrop");
	}
	else if (ActorName_ == es("DroppedItemGroup")) {
		return es("Item");
	}
	return es("error");
}

std::string divisionitem(std::string ItemName) {
	if (ItemName == es("Item_Weapon_AK47_C")) {
		return es("AKM");
	}
	else if (ItemName == es("Item_Weapon_HK416_C")) {
		return es("M416");
	}
	else if (ItemName == es("Item_Weapon_M16A4_C")) {
		return es("M16A4");
	}
	else if (ItemName == es("Item_Weapon_SCAR-L_C")) {
		return es("SCAR-L");
	}
	else if (ItemName == es("Item_Weapon_G36C_C")) {
		return es("G36C");
	}
	else if (ItemName == es("Item_Weapon_UMP_C")) {
		return es("UMP");
	}
	else if (ItemName == es("Item_Weapon_Kar98k_C")) {
		return es("Kar98k");
	}
	else if (ItemName == es("Item_Weapon_M24_C")) {
		return es("M24");
	}
	else if (ItemName == es("Item_Weapon_VSS_C")) {
		return es("VSS");
	}
	else if (ItemName == es("Item_Weapon_Mosin_C")) {
		return es("MosinNagant");
	}
	else if (ItemName == es("Item_Weapon_Mini14_C")) {
		return es("Mini14");
	}
	else if (ItemName == es("Item_Weapon_QBZ95_C")) {
		return es("QBZ95");
	}
	else if (ItemName == es("Item_Weapon_SKS_C")) {
		return es("SKS");
	}
	else if (ItemName == es("Item_Weapon_M249_C")) {
		return es("M249");
	}	
	else if (ItemName == es("Item_Weapon_Mk47Mutant_C")) {
		return es("MK47-Mutant");
	}	
	else if (ItemName == es("Item_Weapon_BerylM762_C")) {
		return es("BerylM762");
	}	
	else if (ItemName == es("Item_Weapon_Vector_C")) {
		return es("Vector");
	}	
	else if (ItemName == es("Item_Weapon_QBU88_C")) {
		return es("QBU");
	}	
	else if (ItemName == es("Item_Armor_D_01_Lv2_C")) {
		return es("Lv.2 Armor");
	}	
	else if (ItemName == es("Item_Armor_C_01_Lv3_C")) {
		return es("Lv.3 Armor");
	}	
	else if (ItemName == es("Item_Head_F_01_Lv2_C")) {
		return es("Lv.2 Helmet");
	}
	else if (ItemName == es("Item_Head_F_02_Lv2_C")) {
		return es("Lv.2 Helmet");
	}
	else if (ItemName == es("Item_Head_G_01_Lv3_C")) {
		return es("Lv.3 Helmet");
	}
	else if (ItemName == es("Item_Back_F_01_Lv2_C")) {
		return es("Lv.2 BackPack");
	}
	else if (ItemName == es("Item_Back_F_02_Lv2_C")) {
		return es("Lv.2 BackPack");
	}
	else if (ItemName == es("Item_Back_C_01_Lv3_C")) {
		return es("Lv.3 BackPack");
	}
	else if (ItemName == es("Item_Back_C_02_Lv3_C")) {
		return es("Lv.3 BackPack");
	}
	else if (ItemName == es("Item_Heal_FirstAid_C")) {
		return es("FirstAidKit");
	}
	else if (ItemName == es("Item_Heal_MedKit_C")) {
		return es("MedKit");
	}
	else if (ItemName == es("Item_Boost_EnergyDrink_C")) {
		return es("EnergyDrink");
	}
	else if (ItemName == es("Item_Boost_PainKiller_C")) {
		return es("PainKiller");
	}
	else if (ItemName == es("Item_Boost_AdrenalineSyringe_C")) {
		return es("AdrenalineSyringe");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_Scope3x_C")) {
		return es("x3 Scope");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_ACOG_01_C")) {
		return es("x4 Scope");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_Scope6x_C")) {
		return es("x6 Scope");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_CQBSS_C")) {
		return es("x8 Scope");
	}
	else if (ItemName == es("Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Large_C")) {
		return es("Ext.QuickDraw-AR");
	}
	else if (ItemName == es("Item_Attach_Weapon_Muzzle_Suppressor_Large_C")) {
		return es("Suppressor-AR");
	}
	else if (ItemName == es("Item_Attach_Weapon_Stock_AR_Composite_C")) {
		return es("Stock-M416/Vector/MP5K");
	}
	else if (ItemName == es("Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C")) {
		return es("Ext.QuickDraw-SR");
	}
	else if (ItemName == es("Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle_C")) {
		return es("Suppressor-SR");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_DotSight_01_C")) {
		return es("RedDot");
	}
	else if (ItemName == es("Item_Attach_Weapon_Upper_Holosight_C")) {
		return es("Hologram");
	}
	else if (ItemName == es("Item_Weapon_Grenade_C")) {
		return es("Grenade");
	}
	else if (ItemName == es("Item_Weapon_SmokeBomb_C")) {
		return es("Smoke");
	}
	else if (ItemName == es("Item_Weapon_FlareGun_C")) {
		return es("FlareGun");
	}
	return es("NULL");
}
#define M_PI	3.14159265358979323846264338327950288419716939937510
#define DEG2RAD(x) x * M_PI / 180.0
AimInfo GetBestTarget() {
	AimInfo a{};
	Vector3 LastTarget = Vector3(0, 0, 0);
	uint64_t pMeshLoc = 0x0;
	uint64_t pRootComp = 0x0;
	uint64_t pActor = 0x0;
	BoneVisibleResult pBoneState;
	Vector3 CrossHair = Vector3(Width / 2, Height / 2, 0);
	int ActorCt = UGameData->GetPInfoCount();
	if (UGameData->LastCachedActorPTR == 0x0) {
		for (int i = 0; i < ActorCt; i++) {
			uint64_t Actor = UGameData->CachedActorArray[i].ActorPawn;
			if (UGameData->CachedActorArray[i].ActorType == Player) {
				int ActorHealth = UGameData->CachedActorArray[i].ActorHealth;
				if (ActorHealth > 0.0) {
					uint64_t ActorMeshAddr = UGameData->CachedActorArray[i].ActorMesh;
					Vector3 ForeHeadPos = Util->GetBoneWithRotation(ActorMeshAddr, Bones::forehead);
					Vector3 vHitLoc = Util->WorldToScreen(ForeHeadPos, UGameData->GetAimCamera());
					if (GetVisBone(GlobalCamera.POV.Location, ForeHeadPos)) {
						cfg->Fov = tanf(DEG2RAD(6.f) / 2) / tanf(DEG2RAD(GlobalCamera.POV.FOV) / 2) * (Width);
						if (vHitLoc.Distance(CrossHair) <= cfg->Fov) {
							if (LastTarget == Vector3(0, 0, 0)) {
								LastTarget = vHitLoc;
								pMeshLoc = ActorMeshAddr;
								pRootComp = Decrypt(mem->RVM<uint64_t>(Actor + RootComponent));
								pActor = Actor;
								pBoneState = UGameData->CachedActorArray[i].pBoneVisible;
							}
							else
							{
								if (vHitLoc.Distance(CrossHair) < LastTarget.Distance(CrossHair)) {
									LastTarget = vHitLoc;
									pMeshLoc = ActorMeshAddr;
									pRootComp = Decrypt(mem->RVM<uint64_t>(Actor + RootComponent));
									pActor = Actor;
									pBoneState = UGameData->CachedActorArray[i].pBoneVisible;
								}
							}
						}
					}
				}
			}
		}
		UGameData->LastCachedActorPTR = pActor;
	}
	else {
		uint64_t actorptr = UGameData->LastCachedActorPTR;
		float ActorHealth = mem->RVM<float>(actorptr + Health);
		if (ActorHealth > 0.0) {
			pMeshLoc = mem->RVM<uint64_t>(actorptr + ActorMesh);
			pActor = actorptr;
		}
		else {
			for (int i = 0; i < ActorCt; i++) {
				uint64_t Actor = UGameData->CachedActorArray[i].ActorPawn;
				if (UGameData->CachedActorArray[i].ActorType == Player) {
					int ActorHealth = UGameData->CachedActorArray[i].ActorHealth;
					if (ActorHealth > 0.0) {
						uint64_t ActorMeshAddr = UGameData->CachedActorArray[i].ActorMesh;
						Vector3 ForeHeadPos = Util->GetBoneWithRotation(ActorMeshAddr, Bones::forehead);
						Vector3 vHitLoc = Util->WorldToScreen(ForeHeadPos, UGameData->GetAimCamera());
						if (GetVisBone(GlobalCamera.POV.Location, ForeHeadPos)) {
							cfg->Fov = tanf(DEG2RAD(6.f) / 2) / tanf(DEG2RAD(GlobalCamera.POV.FOV) / 2) * (Width);
							if (vHitLoc.Distance(CrossHair) <= cfg->Fov) {
								if (LastTarget == Vector3(0, 0, 0)) {
									LastTarget = vHitLoc;
									pMeshLoc = ActorMeshAddr;
									pRootComp = Decrypt(mem->RVM<uint64_t>(Actor + RootComponent));
									pActor = Actor;
									pBoneState = UGameData->CachedActorArray[i].pBoneVisible;
								}
								else
								{
									if (vHitLoc.Distance(CrossHair) < LastTarget.Distance(CrossHair)) {
										LastTarget = vHitLoc;
										pMeshLoc = ActorMeshAddr;
										pRootComp = Decrypt(mem->RVM<uint64_t>(Actor + RootComponent));
										pActor = Actor;
										pBoneState = UGameData->CachedActorArray[i].pBoneVisible;
									}
								}
							}
						}
					}
				}
			}
			UGameData->LastCachedActorPTR = pActor;
		}
	}
	a.pMesh = pMeshLoc;
	a.Actor = pActor;
	a.RootComponent = pRootComp;
	a.BoneState = pBoneState;
	return a;
}

void ExDrawScene() {
	Vector3 center = Vector3(Width / 1.5f, Height - 150, 0);
	int spectated_count = mem->RVM<int>(UGameData->GetLocalPlayerPawn() + SpectatedCount);
	std::string sctext = "Spectator : " + std::to_string(spectated_count);
	DrawStrokeText(ImVec2((Width / 2) - ImGui::CalcTextSize(sctext.c_str()).x, 50), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), sctext.c_str());
	DrawStrokeText(ImVec2(50, 50), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), std::to_string(UGameData->GetBulletSpeed()).c_str());
	cfg->Fov = tanf(DEG2RAD(6.f) / 2) / tanf(DEG2RAD(GlobalCamera.POV.FOV) / 2) * (Width);
	ImGui::GetCurrentWindow()->DrawList->AddCircle(ImVec2(Width / 2, Height / 2), cfg->Fov, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 108, 1.0f);
	int radius = 150;
	if (cfg->Radarcfg) {
		ImGui::GetCurrentWindow()->DrawList->AddCircleFilled(ImVec2(center.x, center.y), radius, ImGui::GetColorU32(ImVec4(0, 0, 0, 0.8)), 256);
		ImGui::GetCurrentWindow()->DrawList->AddCircle(ImVec2(center.x, center.y), radius, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 256);
		ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(center.x, center.y), ImVec2(center.x + cos(GlobalCamera.POV.Rotation.y / 58) * radius, center.y + sin(GlobalCamera.POV.Rotation.y / 58) * radius), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 1.0f);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int ActorCt = UGameData->GetPInfoCount();
	for (int i = 0; i < ActorCt; i++) {
		uint64_t ActorPawn = UGameData->CachedActorArray[i].ActorPawn;
		uint64_t ActorMesh = UGameData->CachedActorArray[i].ActorMesh;
		int ActorTeam = UGameData->CachedActorArray[i].ActorTeam;
		float distance = UGameData->CachedActorArray[i].distance;
		float ActorHealth = UGameData->CachedActorArray[i].ActorHealth;
		Vector3 ActorVector = UGameData->CachedActorArray[i].ActorVector;
		std::string ItemName = UGameData->CachedActorArray[i].ItemName;
		if (UGameData->CachedActorArray[i].ActorType == Player) {
			Vector3 ForeHeadPos = Util->GetBoneWithRotation(ActorMesh, Bones::forehead);
			Vector3 pelvisposition = Util->GetBoneWithRotation(ActorMesh, Bones::pelvis);
			if (cfg->Playercfg) {
				if (ActorTeam >= 100000)
					ActorTeam -= 100000;
				Vector3 InfoTop = Util->WorldToScreen(Vector3(pelvisposition.x, pelvisposition.y, ForeHeadPos.z + 50.0f + distance), UGameData->GetCameraCache());
				DrawInfo(ImVec2(InfoTop.x, InfoTop.y), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 25, (es("[ Team : ") + std::to_string(ActorTeam) + es(" ][ ") + std::to_string((int)distance) + es("M ][ HP : ") + std::to_string((int)ActorHealth) + es(" ]")), distance, ActorHealth);
			}
			if (cfg->Skelcfg) {
				BoneVisibleResult pBoneVisible = UGameData->CachedActorArray[i].pBoneVisible;
				DrawSkeleton(ActorMesh, ImGui::GetColorU32(ImVec4(0, 1, 0, 1)), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), pBoneVisible);
			}
			if (cfg->Radarcfg) {
				Vector3 posfromlocal = pelvisposition - GlobalCamera.POV.Location;
				if (pelvisposition.Distance(GlobalCamera.POV.Location) / 100 <= 200) {
					posfromlocal.x = center.x + ((posfromlocal.x) / 100) * radius / 200;
					posfromlocal.y = center.y + ((posfromlocal.y) / 100) * radius / 200;
					posfromlocal.z = center.z + ((posfromlocal.z) / 100) * radius / 200;
					if (ActorHealth != 0)
						ImGui::GetCurrentWindow()->DrawList->AddCircleFilled(ImVec2(posfromlocal.x, posfromlocal.y), 3, ImGui::GetColorU32(ImVec4(1, 0, 0, 1)));
					else
						ImGui::GetCurrentWindow()->DrawList->AddCircleFilled(ImVec2(posfromlocal.x, posfromlocal.y), 3, ImGui::GetColorU32(ImVec4(0, 0.5, 0, 1)));
				}
			}
		}
		else if (UGameData->CachedActorArray[i].ActorType == AirDrop) {
			Vector3 Loc = Util->WorldToScreen(ActorVector, GlobalCamera);
			DrawStrokeText(ImVec2(Loc.x, Loc.y), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), (es("| AirDrop\n| ") + std::to_string((int)distance) + es("M")).c_str(), 15.0f);
		}
		else if (UGameData->CachedActorArray[i].ActorType == DeathDrop) {
			Vector3 Loc = Util->WorldToScreen(ActorVector, GlobalCamera);
			DrawStrokeText(ImVec2(Loc.x, Loc.y), ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), es("| DeathDrop"), 15.0f);
		}
		else if (UGameData->CachedActorArray[i].ActorType == Item) {
			if (cfg->Itemcfg) {
				if (ItemName == es("NULL"))
					continue;
				else {
					Vector3 Pos = Util->WorldToScreen(ActorVector, GlobalCamera);
					if (distance <= 50)
					{
						DrawStrokeText(ImVec2(Pos.x, Pos.y), ImGui::GetColorU32(ImVec4(0, 1, 1, 1)), (ItemName + " | " + std::to_string((int)distance) + "M").c_str());
					}
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

float ProjectileDrop(float ProjectilESPeed, float m_gravity, float Dist)
{
	if (Dist < 0.001)
		return 0;
	float m_time = Dist / ProjectilESPeed;
	return 0.5 * abs(m_gravity) * m_time * m_time;
}

int AimBot_API(float X, float Y) // 에임봇 마우스이벤트 로직
{
	INPUT InputMouse = { 0 };
	InputMouse.type = INPUT_MOUSE;
	InputMouse.mi.dwFlags = MOUSEEVENTF_MOVE;
	if (abs((LONG)X * cfg->AimSmooth) > 1)
		InputMouse.mi.dx = (LONG)X * (8 * (cfg->AimSmooth / 80));
	if (abs((LONG)Y * cfg->AimSmooth) > 1)
		InputMouse.mi.dy = (LONG)Y * (8 * (cfg->AimSmooth / 80));
	return SendInput(1, &InputMouse, sizeof(INPUT));
}

std::array<int, 17> bonelists{
	Bones::forehead,
	Bones::neck_01,
	Bones::upperarm_l,
	Bones::lowerarm_l,
	Bones::hand_l,
	Bones::upperarm_r,
	Bones::lowerarm_r,
	Bones::hand_r,
	Bones::spine_01,
	Bones::spine_02,
	Bones::pelvis,
	Bones::thigh_l,
	Bones::calf_l,
	Bones::foot_l,
	Bones::thigh_r,
	Bones::calf_r,
	Bones::foot_r
};

void recoilthread() {
	while (1) {
		if (cfg->Aimbot) {
			if (GetAsyncKeyState(VK_RBUTTON) && (int)UGameData->GetCurrentWeaponIndex() <= 3 && (int)UGameData->GetCurrentWeaponIndex() >= 0) {
				Vector3 CrossHair = Vector3(Width / 2, Height / 2, 0);
				AimInfo a = UGameData->GetAimMesh();
				if (a.pMesh != 0x0 && a.Actor != 0x0) {
					uint64_t ActorPTR = a.Actor;
					uint64_t ActorRootComp = Decrypt(mem->RVM<uint64_t>(ActorPTR + RootComponent));
					Vector3 returnvec3 = Util->GetBoneWithRotation(a.pMesh, Bones::forehead);

					bool IsUnique = false;

					if (returnvec3 != Vector3(0, 0, 0) && GetVisBone(GlobalCamera.POV.Location, returnvec3))
					{
						IsUnique = true;
					}
					else 
					{
						for (int i = 0; i < 17; i++)
						{
							returnvec3 = Util->GetBoneWithRotation(a.pMesh, bonelists[i]);
							if (returnvec3 != Vector3(0, 0, 0) && GetVisBone(GlobalCamera.POV.Location, returnvec3))
							{
								IsUnique = true;
								break;
							}
						}
					}

					if (IsUnique) {
						uint64_t pVehicleRiderComponent = mem->RVM<uint64_t>(ActorPTR + VehicleRiderComponent);
						uint64_t pLastVehiclePawn = mem->RVM<uint64_t>(pVehicleRiderComponent + LastVehiclePawn);
						Vector3 VehicleVelocity = mem->ReadVector3(pLastVehiclePawn + ReplicatedMovement);
						float flytime = (returnvec3.Distance(UGameData->GetCameraCache().POV.Location) / 100) / UGameData->GetBulletSpeed();
						returnvec3.x = returnvec3.x + (VehicleVelocity.x * flytime);
						returnvec3.y = returnvec3.y + (VehicleVelocity.y * flytime);
						returnvec3.z = returnvec3.z + (VehicleVelocity.z * flytime);

						Vector3 Velocity3 = mem->ReadVector3(ActorRootComp + ActorVelocity);
						returnvec3.x = returnvec3.x + (Velocity3.x * flytime);
						returnvec3.y = returnvec3.y + (Velocity3.y * flytime);
						returnvec3.z = returnvec3.z + (Velocity3.z * flytime);
						if (GlobalCamera.POV.Location.Distance(returnvec3) / 100 >= 300) {
							Vector3 vecRelativePos = GlobalCamera.POV.Location.operator-(returnvec3);
							float lDeltaInMeters = (float)vecRelativePos.Length() / 45;
							float drop = ProjectileDrop(UGameData->GetBulletSpeed(), -9.8 * 10, lDeltaInMeters);
							returnvec3.z += drop;
						}

						Vector3 ALOC = Util->WorldToScreen(returnvec3, UGameData->GetAimCamera());
						Vector3 vHitLoc = ALOC - CrossHair;
						vHitLoc.x /= M_PI;
						vHitLoc.y /= M_PI;
						//AimBot_API(vHitLoc.x, vHitLoc.y);
						mem->WVM<float>(UGameData->GetLocalPlayerController() + InputRotation, (-vHitLoc.y / (8 * (80 / UGameData->GetCameraCache().POV.FOV))) * cfg->AimSmooth);
						mem->WVM<float>(UGameData->GetLocalPlayerController() + InputRotation + 0x4, (vHitLoc.x / (8 * (80 / UGameData->GetCameraCache().POV.FOV))) * cfg->AimSmooth);
					}
				}
			}
		}
		Sleep(1);
	}
}

bool islogin = false;
char identify[32]{};

#include <winhttp.h>
#include <urlmon.h>
#include <thread>
#include <random>
#include "imgui_impl_win32.h"
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "winhttp.lib")

static const char alphanum[] =
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char genRandom()
{
	return alphanum[rand() % stringLength];
}


std::string encrypt(std::string msg, std::string key) {
	std::string tmp(key);
	while (key.size() < msg.size()) key += tmp;
	for (std::string::size_type i = 0; i < msg.size(); ++i) msg[i] ^= key[i];
	return msg;
}

std::wstring get_utf16(const std::string& str, int codepage)
{
	if (str.empty()) return std::wstring();
	int sz = MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), 0, 0);
	std::wstring res(sz, 0);
	MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), &res[0], sz);
	return res;
}

std::string WebWinhttp(std::string details)
{
	//VMProtectBeginUltra("WebFunc");
	DWORD dwSize = 0, dwDownloaded;
	LPSTR source;
	source = (char*)"";
	std::string responsed = "";

	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResults = FALSE;

	hSession = WinHttpOpen(L"Winhttp API", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hSession)
		hConnect = WinHttpConnect(hSession, get_utf16("skyhost.kr", CP_UTF8).c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);

	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", get_utf16(details, CP_UTF8).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, L"content-type:application/x-www-form-urlencoded", -1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	if (bResults) {
		do {
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u", GetLastError());

			source = (char*)malloc(dwSize + 1);
			if (!source) {
				printf("Out of memory\n");
				dwSize = 0;
			}
			else {
				ZeroMemory(source, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)source, dwSize, &dwDownloaded))
					printf("Error %u", GetLastError());
				else
					responsed = responsed + source;
				free(source);
			}
		} while (dwSize > 0);
	}

	if (hRequest)WinHttpCloseHandle(hRequest);
	if (hConnect)WinHttpCloseHandle(hConnect);
	if (hSession)WinHttpCloseHandle(hSession);
	return responsed;
	//VMProtectEnd();
}

std::string CodeVal, decrypted, decoded;

int outcount = 0;

void Htbeat() {
	while (1) {
		std::string hash = "";
		srand(time(0));
		for (int i = 0; i < 32; ++i) {
			hash += genRandom();
		}
		std::string a = WebWinhttp(es("/api.php?mode=login&Code=") + cfg->License + es("&otp=") + hash + es("&COM=") + GetProcessorId());
		std::string b = encrypt(a, hash);
		if (b.find(es("Success : ")) != std::string::npos)
		{
			cfg->remainingtime = b.std::string::substr(10);
		}
		else if (b.find(es("Expired License")) != std::string::npos) {
			islogin = false;
			MessageBox(GetActiveWindow(), es("Expired licenses, Please purchase additional licenses for your service."), es("Thank you!"), MB_ICONINFORMATION);
			a.clear();
			b.clear();
			ExitThread(0);
		}
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}
}

long __stdcall hk_present(IDXGISwapChain* p_swapchain, unsigned int syncintreval, unsigned int flags)
{
	if (!isInit) {
		p_swapchain->GetDevice(__uuidof(pD3D11_Device), (void**)&pD3D11_Device);
		pD3D11_Device->GetImmediateContext(&pD3D11_Context);

		D3D11_VIEWPORT vp;
		UINT nvp = 1;
		pD3D11_Context->RSGetViewports(&nvp, &vp);
		Width = vp.Width;
		Height = vp.Height;

		DXGI_SWAP_CHAIN_DESC sd;
		p_swapchain->GetDesc(&sd);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontFromFileTTF(es("C:\\Windows\\Fonts\\malgunbd.TTF"), 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		window = sd.OutputWindow;

		ImGui_ImplDX11_Init(pD3D11_Device, pD3D11_Context);
		ImGui_ImplWin32_Init(window);
		ImGui::GetIO().ImeWindowHandle = window;
		//SetWindowDisplayAffinity(window, WDA_MONITOR);
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

		OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hWndProc);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)recoilthread, 0, 0, 0);

		isInit = true;
	};

	ID3D11Texture2D* pBackBuffer;
	p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);//This = Swapchain
	ID3D11RenderTargetView* pRTV;
	pD3D11_Device->CreateRenderTargetView(pBackBuffer, NULL, &pRTV);
	pD3D11_Context->OMSetRenderTargets(1, &pRTV, NULL);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if(!islogin) {
		ImGui::SetNextWindowSize(ImVec2(300, 200), 0);
		ImGui::Begin(es("LAST - PUBG"), &g_ShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		//ImGui::InputText(es("License"), identify, 32);
		if (ImGui::Button(es("Injection"), ImVec2(95, 25))) {
			std::string Hash = "";
			srand(time(0));
			for (int i = 0; i < 32; ++i) {
				Hash += genRandom();
			}
			std::string a(identify);
			//CodeVal = WebWinhttp(es("/api.php?mode=login&Code=") + a + es("&otp=") + Hash + es("&COM=") + GetProcessorId());
			//decrypted = encrypt(CodeVal, Hash);
			//if (decrypted.find(es("Success : ")) != std::string::npos)
			{
				//std::string a(identify);
				//cfg->License = a;
				//cfg->remainingtime = decrypted.std::string::substr(10);
				//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Htbeat, 0, 0, 0);
				islogin = true;
			}
			/*else if (decrypted.find(es("Expired License")) != std::string::npos) {
				MessageBox(GetActiveWindow(), es("Expired licenses, Please purchase additional licenses for your service."), es("Thank you!"), MB_ICONINFORMATION);
			}
			else {
				MessageBox(GetActiveWindow(), es("Unexpected Error"), es("Sorry."), MB_ICONWARNING);
			}*/
		}
		ImGui::End();
	}
	else {
		if (is_bitbit_called)
		{
			if (g_ShowMenu) {
				ImGui::SetNextWindowSize(ImVec2(300, 275), 0);
				ImGui::Begin(es("Stealth"), &g_ShowMenu);

				if (ImGui::BeginTabBar(u8"##menu", ImGuiTabBarFlags_None))
				{
					if (ImGui::BeginTabItem(u8"Visuals"))
					{
						ImGui::Checkbox(es("Player"), &cfg->Skelcfg);
						ImGui::Checkbox(es("Player Info"), &cfg->Playercfg);
						ImGui::Checkbox(es("Radar"), &cfg->Radarcfg);
						ImGui::Checkbox(es("Item"), &cfg->Itemcfg);

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem(u8"Aimbot"))
					{
						ImGui::Checkbox(es("No Recoil / A1mBot"), &cfg->Aimbot);
						ImGui::SliderFloat("AimSmooth", &cfg->AimSmooth, 0.00000f, 1.f, "%0.2f");
						ImGui::SliderFloat("AimFov", &cfg->Fov, 0.00000f, 500.0f, "%.1f");

						ImGui::EndTabItem();
					}
				}
				//ImGui::Text(("Remaining Time : " + cfg->remainingtime).c_str());
				//ImGui::Text((es("FPS - ") + std::to_string((int)ImGui::GetIO().Framerate)).c_str());
				ImGui::End();
			}
			//DrawScene();
			ImGui::SetNextWindowPos(ImVec2(0, 0), 1);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, 1);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::Begin(es("backbuffer"), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

			ExDrawScene();

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->PushClipRectFullScreen();
			ImGui::PopStyleColor();
			ImGui::End();
		}
	}
	ImGui::EndFrame();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	D3D11_TEXTURE2D_DESC backBufferDesc;
	pBackBuffer->GetDesc(&backBufferDesc);
	pBackBuffer->Release();

	return reinterpret_cast<decltype(&hk_present)>(m_OriginalPresent)(p_swapchain, syncintreval, flags);
}

void HookD3D11()
{
	m_pSwapChain = GetSwapChain();
	if (m_pSwapChain)
	{
		DWORD_PTR* pSwapChainVtable = (DWORD_PTR*)m_pSwapChain;
		pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];
		o_present = (present_fn)(DWORD_PTR*)pSwapChainVtable[8];
		HMODULE hModule = LoadLibrary(TEXT(es("Gdi32.dll")));
		o_BitBlt = (Bitb)GetProcAddress(hModule, es("BitBlt"));
		MH_Initialize();
		//printf("MH_Initlaize!\n");
		MH_CreateHook((LPVOID)o_present, &hk_present, &m_OriginalPresent);
		MH_CreateHook((LPVOID)o_BitBlt, &Hooked_BitBlt, &m_OriginalBitBlt);
		MH_EnableHook(MH_ALL_HOOKS);
		//printf("ALL HOOK Success!\n");
	}
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;

	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_HOME)
		{
			g_ShowMenu = !g_ShowMenu;
		}

	}

	if (g_ShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}
