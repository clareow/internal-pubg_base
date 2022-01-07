#include "UGameMemory.h"
#include "MemoryManager.h"
#include "Decrypt.h"
#include "offset.h"
#include "defines.h"
#include "utils.h"
#include <Windows.h>
#include <cstdint>
#include <string>
#include "xorstr.h"
#include "D3DHOOK.h"
#include "UE4Func.h"

uint64_t UGameMemory::GetGNames() {
	return GNamesAddr;
}

uint64_t UGameMemory::GetUWorld() {
	return UWorldAddr;
}

uint64_t UGameMemory::GetULevel() {
	return ULevelAddr;
}

uint64_t UGameMemory::GetGInstance() {
	return GInstAddr;
}

uint64_t UGameMemory::GetActors() {
	return ActorTArray;
}

uint64_t UGameMemory::GetLocalPlayer() {
	return LocalPlayerAddr;
}

uint64_t UGameMemory::GetLocalPlayerPawn() {
	return LocalPlayerPawnAddr;
}

uint64_t UGameMemory::GetLocalPlayerController() {
	return LocalPlayerControllerAddr;
}

uint64_t UGameMemory::GetCameraManager() {
	return CameraManagerAddr;
}

void UGameMemory::GetActorMeshArray(uint64_t aCopy[100]){
	memcpy(&aCopy, &CachedActorMeshArray, sizeof(CachedActorMeshArray));
}

void UGameMemory::GetActorsArray(pActorInfo aCopy[2000]) {
	memcpy(&aCopy, &CachedActorArray, sizeof(CachedActorArray));
}

int UGameMemory::GetActorCount() {
	return ActorCount;
}

int UGameMemory::GetPInfoCount() {
	return pInfoActorCount;
}

AimInfo UGameMemory::GetAimMesh() {
	AimInfo a;
	a.pMesh = CachedAimMesh;
	a.Actor = CachedAimActor;
	a.RootComponent = CachedAimRootComp;
	return a;
}

FCameraCacheEntry UGameMemory::GetCameraCache() {
	
	FCameraCacheEntry returnval{};
	returnval.POV.FOV = mem->RVM<float>(CameraManagerAddr + CameraFOV);
	returnval.POV.Location = mem->ReadVector3(CameraManagerAddr + CameraLocation);
	returnval.POV.Rotation = mem->ReadVector3(CameraManagerAddr + CameraRotation);
	return returnval;
}

FCameraCacheEntry UGameMemory::GetAimCamera() {
	
	FCameraCacheEntry returnval{};
	uint64_t LocalPlayerMeshAddr = mem->RVM<uint64_t>(LocalPlayerPawnAddr + ActorMesh);
	uint64_t LocalAnimInstPTR = mem->RVM<uint64_t>(LocalPlayerMeshAddr + AnimScriptInstance);
	Vector3 Recoil = mem->ReadVector3(LocalAnimInstPTR + RecoilAdsRotation_CP);
	Vector3 Sway = mem->ReadVector3(LocalAnimInstPTR + ControlRotation_CP);
	returnval.POV.FOV = mem->RVM<float>(CameraManagerAddr + CameraFOV);
	returnval.POV.Location = mem->ReadVector3(CameraManagerAddr + CameraLocation);
	returnval.POV.Rotation = Sway + Recoil;
	return returnval;
}

BYTE UGameMemory::GetCurrentWeaponIndex() {
	return CurrentWeaponIndex;
}

float UGameMemory::GetBulletSpeed() {
	return BulletSpeedVal;
}

void UGameMemory::UpdateAddress() {
	BaseAddr = (uint64_t)GetModuleHandleA(NULL);
	GNamesAddr = mem->RVM<uint64_t>(BaseAddr + GNames);
	UWorldAddr = Decrypt(mem->RVM<uint64_t>(BaseAddr + UWorld));
	ULevelAddr = Decrypt(mem->RVM<uint64_t>(UWorldAddr + PersistentLevel));
	LocalPlayerAddr = mem->RVM<uint64_t>(BaseAddr + LocalPlayer);
	LocalPlayerControllerAddr = Decrypt(mem->RVM<uint64_t>(LocalPlayerAddr + PlayerController));
	LocalPlayerPawnAddr = Decrypt(mem->RVM<uint64_t>(LocalPlayerControllerAddr + AcknowledgedPawn));
	ActorTArray = Decrypt(mem->RVM<uint64_t>(ULevelAddr + ActorsArray));
	ActorCount = mem->RVM<int>(ActorTArray + 0x8);
	CameraManagerAddr = mem->RVM<uint64_t>(LocalPlayerControllerAddr + PlayerCameraManager);
	LocalPlayer_WeaponProcessor = mem->RVM<uint64_t>(LocalPlayerPawnAddr + WeaponProcessor);
	LocalPlayer_EquippedWeapon = mem->RVM<uint64_t>(LocalPlayer_WeaponProcessor + EquippedWeapons);
	CurrentWeaponIndex = mem->RVM<BYTE>(LocalPlayer_WeaponProcessor + EquippedWeapons + 0x21);
	LocalPlayer_CurrentWeapon = mem->RVM<uint64_t>(LocalPlayer_EquippedWeapon + (uint64_t(CurrentWeaponIndex) * 0x8));
	LocalPlayer_WeaponData = mem->RVM<uint64_t>(LocalPlayer_CurrentWeapon + WeaponTrajectoryData);
	BulletSpeedVal = mem->RVM<float>(LocalPlayer_WeaponData + TrajectoryConfig);
}

BoneVisibleResult CheckBoneStatus(DWORD_PTR pMesh) {
	BoneVisibleResult integer;
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
	//integer.forehead = false;
	//integer.upperarm_l = false;
	//integer.arm_l = false;
	//integer.hand_l = false;
	//integer.upperarm_r = false;
	//integer.arm_r = false;
	//integer.hand_r = false;
	//integer.spine2 = false;
	//integer.spine1 = false;
	//integer.pelvis = false;
	//integer.thigh_l = false;
	//integer.calf_l = false;
	//integer.foot_l = false;
	//integer.thigh_r = false;
	//integer.calf_r = false;
	//integer.foot_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rforehead))
		integer.forehead = true;
	else
		integer.forehead = false;
	if (GetVisBone(GlobalCamera.POV.Location, rupperarm_l))
		integer.upperarm_l = true;
	else
		integer.upperarm_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rarm_l))
		integer.arm_l = true;
	else
		integer.arm_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rhand_l))
		integer.hand_l = true;
	else
		integer.hand_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rupperarm_r))
		integer.upperarm_r = true;
	else
		integer.upperarm_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rarm_r))
		integer.arm_r = true;
	else
		integer.arm_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rhand_r))
		integer.hand_r = true;
	else
		integer.hand_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rspine2))
		integer.spine2 = true;
	else
		integer.spine2 = false;
	if (GetVisBone(GlobalCamera.POV.Location, rspine1))
		integer.spine1 = true;
	else
		integer.spine1 = false;
	if (GetVisBone(GlobalCamera.POV.Location, rpelvis))
		integer.pelvis = true;
	else
		integer.pelvis = false;
	if (GetVisBone(GlobalCamera.POV.Location, rthigh_l))
		integer.thigh_l = true;
	else
		integer.thigh_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rcalf_l))
		integer.calf_l = true;
	else
		integer.calf_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rfoot_l))
		integer.foot_l = true;
	else
		integer.foot_l = false;
	if (GetVisBone(GlobalCamera.POV.Location, rthigh_r))
		integer.thigh_r = true;
	else
		integer.thigh_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rcalf_r))
		integer.calf_r = true;
	else
		integer.calf_r = false;
	if (GetVisBone(GlobalCamera.POV.Location, rfoot_r))
		integer.foot_r = true;
	else
		integer.foot_r = false;
	return integer;
}

void UGameMemory::UpdateActors() {
	int j = 0;
	pActorInfo pCopyActorInfo[2000] = {};
	uint64_t Actor_ = mem->RVM<uint64_t>(ActorTArray + 0x0);
	for (int i = 0; i < ActorCount; i++) {
		uint64_t Actor = mem->RVM<uint64_t>(Actor_ + i * 0x8);
		int ActorsId = Decrypt_CIndex(mem->RVM<int>(Actor + ActorID));
		std::string ActorName_ = Util->getName(ActorsId);
		int actorTeam = mem->RVM<int>(Actor + LastTeamNum);
		int LocTeamNum = mem->RVM<int>(UGameData->GetLocalPlayerPawn() + LastTeamNum);
		uint64_t RootComp = Decrypt(mem->RVM<uint64_t>(Actor + RootComponent));
		Vector3 Loc_ = mem->ReadVector3(RootComp + absoluteLocation);
		float distance = Loc_.Distance(GlobalCamera.POV.Location) / 100;
		if (actorTeam == LocTeamNum)
			continue;
		if (divisionesp(ActorName_) == es("Player")) {
			uint64_t MeshComp_ = mem->RVM<uint64_t>(Actor + ActorMesh);
			CachedActorArray[j].ActorPawn = Actor;
			CachedActorArray[j].ActorMesh = MeshComp_;
			CachedActorArray[j].ActorTeam = actorTeam;
			CachedActorArray[j].distance = distance;
			CachedActorArray[j].ActorHealth = mem->RVM<float>(Actor + Health);
			CachedActorArray[j].pBoneVisible = CheckBoneStatus(MeshComp_);
			CachedActorArray[j].ActorType = Player;
			j++;
		}
		else if (divisionesp(ActorName_) == es("Airdrop")) {
			CachedActorArray[j].ActorPawn = Actor;
			CachedActorArray[j].distance = distance;
			CachedActorArray[j].ActorVector = Loc_;
			CachedActorArray[j].ActorType = AirDrop;
			j++;
		}
		else if (divisionesp(ActorName_) == es("DeathDrop")) {
			CachedActorArray[j].ActorPawn = Actor;
			CachedActorArray[j].distance = distance;
			CachedActorArray[j].ActorVector = Loc_;
			CachedActorArray[j].ActorType = DeathDrop;
			j++;
		}
		else if (divisionesp(ActorName_) == es("Item")) {
			uint64_t pItemArray = mem->RVM<uint64_t>(Actor + DroppedItemGroup);
			int pItemCount = mem->RVM<int>(Actor + DroppedItemGroup + 0x8);
			std::string ItemName = es("NULL");

			if (pItemCount > 100)
				continue;

			Vector3 Pos_ = Vector3();
			float Distance = Pos_.Distance(GlobalCamera.POV.Location) / 100;
			for (int j = 0; j < pItemCount; j++)
			{
				uint64_t pItemObject = mem->RVM<uint64_t>(pItemArray + (j * 0x10));
				uint64_t pUItemAddress = mem->RVM<uint64_t>(pItemObject + DroppedItemGroup_UItem);
				int pUItemID = mem->RVM<int>(pUItemAddress + ItemID);
				if (pUItemID > 0 && pUItemID < 399999)
				{
					Pos_ = mem->ReadVector3(pItemObject + absoluteLocation);
					Distance = Pos_.Distance(GlobalCamera.POV.Location) / 100;
					if (divisionitem(Util->getName(pUItemID)) == es("NULL"))
						continue;
					else {
						ItemName = divisionitem(Util->getName(pUItemID));
					}
				}
			}
		}
	}
	pInfoActorCount = j;
	memcpy(&CachedActorArray, &pCopyActorInfo, sizeof(pCopyActorInfo));
}

UGameMemory *UGameData = new UGameMemory;