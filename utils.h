#pragma once
#include <vector>
#include <string>
#include <d3dtypes.h>
#include "defines.h"

class Utils {
public:
	bool inited = false;

	std::vector<std::string> GNamesStr{};

	D3DMATRIX Matrix(Vector3 rot, Vector3 origin);

	Vector3 WorldToScreen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL);

	D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2);

	FTransform GetBoneIndex(DWORD_PTR mesh, int index);

	Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id);

	std::string getName(int id);

	std::string GetNameFromIdUsingGName(uint64_t GN, int ID);

	uint64_t FindGnames();
};

extern Utils* Util;