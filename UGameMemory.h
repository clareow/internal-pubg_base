#pragma once
#include <cstdint>
#include "defines.h"

class UGameMemory {
public:
	uint64_t GetGNames();
	uint64_t GetUWorld();
	uint64_t GetULevel();
	uint64_t GetGInstance();
	uint64_t GetLocalPlayer();
	uint64_t GetActors();
	AimInfo GetAimMesh();
	int GetActorCount();
	int GetPInfoCount();
	uint64_t GetLocalPlayerController();
	uint64_t GetLocalPlayerPawn();
	uint64_t GetCameraManager();
	void GetActorMeshArray(uint64_t aCopy[100]);
	void GetActorsArray(pActorInfo aCopy[2000]);
	FCameraCacheEntry GetCameraCache();
	FCameraCacheEntry GetAimCamera();
	BYTE GetCurrentWeaponIndex();
	float GetBulletSpeed();
	void UpdateAddress();
	void UpdateActors();
	uint64_t CachedActorMeshArray[100]{};
	pActorInfo CachedActorArray[2000]{};
	uint64_t CachedAimMesh{};
	uint64_t CachedAimActor{};
	uint64_t CachedAimRootComp{};
	BoneVisibleResult CachedAimBone{};
	uint64_t LastCachedActorPTR{};
private:
	int ActorCount{};
	int pInfoActorCount{};
	uint64_t BaseAddr{};
	uint64_t GNamesAddr{};
	uint64_t UWorldAddr{};
	uint64_t ULevelAddr{};
	uint64_t GInstAddr{};
	uint64_t ActorTArray{};
	uint64_t LocalPlayerAddr{};
	uint64_t LocalPlayerControllerAddr{};
	uint64_t LocalPlayerPawnAddr{};
	uint64_t CameraManagerAddr{};
	uint64_t LocalPlayer_WeaponProcessor{};
	uint64_t LocalPlayer_EquippedWeapon{};
	BYTE CurrentWeaponIndex{};
	uint64_t LocalPlayer_CurrentWeapon{};
	uint64_t LocalPlayer_WeaponData{};
	float BulletSpeedVal{};
};

extern UGameMemory *UGameData;