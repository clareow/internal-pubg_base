#pragma once
#include "utils.h"
#include "defines.h"
#include "offset.h"
#include "Decrypt.h"
#include "Memory.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <corecrt_math_defines.h>
#include "MemoryManager.h"
#include "UGameMemory.h"
#include "D3DHOOK.h"
#include "xorstr.h"

D3DMATRIX Utils::Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix._11 = CP * CY;
	matrix._12 = CP * SY;
	matrix._13 = SP;
	matrix._14 = 0.f;

	matrix._21 = SR * SP * CY - CR * SY;
	matrix._22 = SR * SP * SY + CR * CY;
	matrix._23 = -SR * CP;
	matrix._24 = 0.f;

	matrix._31 = -(CR * SP * CY + SR * SY);
	matrix._32 = CY * SR - CR * SP * SY;
	matrix._33 = CR * CP;
	matrix._34 = 0.f;

	matrix._41 = origin.x;
	matrix._42 = origin.y;
	matrix._43 = origin.z;
	matrix._44 = 1.f;

	return matrix;
}

Vector3 Utils::WorldToScreen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCacheL.POV;
	Vector3 Rotation = POV.Rotation; // FRotator

	D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix._11, tempMatrix._12, tempMatrix._13);
	vAxisY = Vector3(tempMatrix._21, tempMatrix._22, tempMatrix._23);
	vAxisZ = Vector3(tempMatrix._31, tempMatrix._32, tempMatrix._33);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = Width / 2.0f;
	float ScreenCenterY = Height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

bool inited = false;

D3DMATRIX Utils::MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}


FTransform Utils::GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = mem->RVM<uint64_t>(mesh + BoneArray);

	return mem->ReadFTransform(bonearray + (index * 0x30));
}

Vector3 Utils::GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);

	FTransform ComponentToWorld_ = mem->ReadFTransform(mesh + ComponentToWorld);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld_.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

std::string Utils::getName(int id) {
	std::string returnv = std::string(std::to_string(GNamesStr.size()) + es(" | ") + std::to_string(id));
	if (GNamesStr.size() > id)
		returnv = GNamesStr[id];
	return returnv;
}

std::string Utils::GetNameFromIdUsingGName(uint64_t GN, int ID)
{
	UINT64 fNamePtr = mem->RVM<uint64_t>(GN + ((int(ID / ChunkSize)) * 8));
	if (fNamePtr == 0) {
		return es("fail");
	}

	UINT64 fName = mem->RVM<uint64_t>(fNamePtr + ((int(ID % ChunkSize)) * 8));
	if (fName == 0) {
		return es("fail");
	}

	char names_c[64];
	if (mem->READ_STRING(fName + 0x10, names_c))
		return es("fail");

	std::string names = std::string(names_c);
	if (names == es(""))
		names = es("fail");

	return names;
}

uint64_t Utils::FindGnames() {
    UINT64 gnames = 0;
    UINT64 tmp = UGameData->GetGNames();
    DWORD64 v11 = Decrypt(tmp);
    v11 = mem->RVM<uint64_t>(v11);

    DWORD64 v12 = Decrypt(v11);
    DWORD64 v13 = mem->RVM<uint64_t>(v12);
    v13 = Decrypt(v13);
    tmp = Decrypt(mem->RVM<uint64_t>(v13));

    gnames = 0;
#pragma region NameDumper

    gnames = tmp;
    if (GetNameFromIdUsingGName(gnames, 1) == es("ByteProperty") && GetNameFromIdUsingGName(gnames, 2) == es("IntProperty") && GetNameFromIdUsingGName(gnames, 3) == es("BoolProperty"))
    {
#pragma region NameDumper
        int ia = 0;
        int fails = 0;
        while (1) {

            std::string Gres = GetNameFromIdUsingGName(gnames, ia);

            if (Gres == es("fail")) {
                GNamesStr.push_back(Gres);
                fails++;
            }
            else {
                fails = 0;
                GNamesStr.push_back(Gres);
            }

            if (ia > 400000) {
                break;
            }
            ia++;
        }
#pragma endregion
        return gnames;
    }
    MessageBox(NULL, es("Reboot PUBG"), es("Some Error Occured"), MB_ICONERROR);
    exit(1);
    return 0;
}

Utils* Util = new Utils;