#pragma once
#include <Windows.h>
#include <d3dtypes.h>
#include <algorithm>
#include <string>

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}


	inline float Length()
	{
		float ls = Dot(Vector3{ x, y, z });
		return (float)sqrt(ls);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator/(float f) {
		return Vector3(x / f, y / f, z / f);
	}

	bool operator!=(Vector3 v)
	{
		if (x != v.x)
			return true;
		if (y != v.y)
			return true;
		if (z != v.z)
			return true;
		return false;
	}
	bool operator==(Vector3 v)
	{
		if (x != v.x)
			return false;
		if (y != v.y)
			return false;
		if (z != v.z)
			return false;
		return true;
	}
	bool operator>(Vector3 v)
	{
		if (x > v.x)
			return false;
		if (y > v.y)
			return false;
		if (z > v.z)
			return false;
		return true;
	}
	bool operator<(Vector3 v)
	{
		if (x < v.x)
			return false;
		if (y < v.y)
			return false;
		if (z < v.z)
			return false;
		return true;
	}
};

struct BoneVisibleResult {
	bool forehead;
	bool neck;
	bool upperarm_l;
	bool arm_l;
	bool hand_l;
	bool upperarm_r;
	bool arm_r;
	bool hand_r;
	bool spine1;
	bool spine2;
	bool pelvis;
	bool thigh_l;
	bool calf_l;
	bool foot_l;
	bool thigh_r;
	bool calf_r;
	bool foot_r;
};

enum pType {
	Player = 0x0,
	AirDrop = 0x1,
	DeathDrop = 0x2,
	Item = 0x3
};

struct pActorInfo {
	uint64_t ActorPawn;
	uint64_t ActorMesh;
	Vector3 ActorVector;
	float distance;
	float ActorHealth;
	int ActorTeam;
	std::string ItemName;
	pType ActorType;
	BoneVisibleResult pBoneVisible;
};

struct FMinimalViewInfo
{
	class Vector3                                      Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class Vector3                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0024(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              AspectRatio;                                              // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

struct FCameraCacheEntry
{
	float                                              TimeStamp;                                                // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0xC];                                       // 0x0004(0x000C) MISSED OFFSET
	struct FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530)
};

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FHitResult
{
	unsigned char                                      bBlockingHit : 1;                                         // 0x0000(0x0001)
	unsigned char                                      bStartPenetrating : 1;                                    // 0x0000(0x0001)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
	float                                              Time;                                                     // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
	float                                              Distance;                                                 // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)                                            // 0x0048(0x000C)
	float                                              PenetrationDepth;                                         // 0x0054(0x0004) (ZeroConstructor, IsPlainOldData)
	int                                                Item;                                                     // 0x0058(0x0004) (ZeroConstructor, IsPlainOldData)                                               // 0x006C(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x0074(0x0004) MISSED OFFSET                                                // 0x0078(0x0008) (ZeroConstructor, IsPlainOldData)
	int                                                FaceIndex;                                                // 0x0080(0x0004) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData02[0x4];                                       // 0x0084(0x0004) MISSED OFFSET
	unsigned char                                      UnknownData066[0xFF];                                       // 0x0084(0x0004) MISSED OFFSET
};


enum class ETraceTypeQuery : uint8_t {
	TraceTypeQuery1 = 0,
	TraceTypeQuery2 = 1,
	TraceTypeQuery3 = 2,
	TraceTypeQuery4 = 3,
	TraceTypeQuery5 = 4,
	TraceTypeQuery6 = 5,
	TraceTypeQuery7 = 6,
	TraceTypeQuery8 = 7,
	TraceTypeQuery9 = 8,
	TraceTypeQuery10 = 9,
	TraceTypeQuery11 = 10,
	TraceTypeQuery12 = 11,
	TraceTypeQuery13 = 12,
	TraceTypeQuery14 = 13,
	TraceTypeQuery15 = 14,
	TraceTypeQuery16 = 15,
	TraceTypeQuery17 = 16,
	TraceTypeQuery18 = 17,
	TraceTypeQuery19 = 18,
	TraceTypeQuery20 = 19,
	TraceTypeQuery21 = 20,
	TraceTypeQuery22 = 21,
	TraceTypeQuery23 = 22,
	TraceTypeQuery24 = 23,
	TraceTypeQuery25 = 24,
	TraceTypeQuery26 = 25,
	TraceTypeQuery27 = 26,
	TraceTypeQuery28 = 27,
	TraceTypeQuery29 = 28,
	TraceTypeQuery30 = 29,
	TraceTypeQuery31 = 30,
	TraceTypeQuery32 = 31,
	TraceTypeQuery_MAX = 32,
	ETraceTypeQuery_MAX = 33
};

enum class EDrawDebugTrace : uint8_t {
	EDrawDebugTrace__None = 0,
	EDrawDebugTrace__ForOneFrame = 1,
	EDrawDebugTrace__ForDuration = 2,
	EDrawDebugTrace__Persistent = 3,
	EDrawDebugTrace__EDrawDebugTrace_MAX = 4
};

struct FLinearColor
{
	float                                              R;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              G;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              B;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              A;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FLinearColor()
		: R(0), G(0), B(0), A(0)
	{ }

	inline FLinearColor(float r, float g, float b, float a)
		: R(r),
		G(g),
		B(b),
		A(a)
	{ }

};

template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline TArray(T* data, int32_t number)
	{
		Data = data;
		Count = Max = number;
	};

	inline void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct FScriptMulticastDelegate
{
	char UnknownData[0x10];
};

class AActor
{
public:
	unsigned char                                      UnknownData00[0x18];                                      // 0x0028(0x0018) MISSED OFFSET                                      // 0x0040(0x0058) (Edit, DisableEditOnInstance)
	float                                              CustomTimeDilation;                                       // 0x0098(0x0004) (BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      bHidden : 1;                                              // 0x009C(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, Net)
	unsigned char                                      bNetTemporary : 1;                                        // 0x009C(0x0001)
	unsigned char                                      bNetStartup : 1;                                          // 0x009C(0x0001)
	unsigned char                                      bOnlyRelevantToOwner : 1;                                 // 0x009C(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
	unsigned char                                      bAlwaysRelevant : 1;                                      // 0x009C(0x0001) (Edit, BlueprintVisible, DisableEditOnInstance)
	unsigned char                                      bReplicateMovement : 1;                                   // 0x009C(0x0001) (Edit, Net, DisableEditOnInstance)
	unsigned char                                      bTearOff : 1;                                             // 0x009C(0x0001) (Net)
	unsigned char                                      bExchangedRoles : 1;                                      // 0x009C(0x0001) (Transient)
	unsigned char                                      UnknownData01 : 1;                                        // 0x009D(0x0001)
	unsigned char                                      bNetLoadOnClient : 1;                                     // 0x009D(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      bNetUseOwnerRelevancy : 1;                                // 0x009D(0x0001) (Edit, BlueprintVisible, DisableEditOnInstance)
	unsigned char                                      bBlockInput : 1;                                          // 0x009D(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      UnknownData02 : 1;                                        // 0x009D(0x0001)
	unsigned char                                      bAllowTickBeforeBeginPlay : 1;                            // 0x009D(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      UnknownData03 : 2;                                        // 0x009D(0x0001)
	unsigned char                                      UnknownData04 : 1;                                        // 0x009E(0x0001)
	unsigned char                                      bActorEnableCollision : 1;                                // 0x009E(0x0001)
	unsigned char                                      UnknownData05 : 1;                                        // 0x009E(0x0001)
	unsigned char                                      bReplicates : 1;                                          // 0x009E(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
	unsigned char                                      UnknownData06[0x1];                                       // 0x009F(0x0001) MISSED OFFSET
	unsigned char                                      UnknownData10[0x3];                                       // 0x0141(0x0003) MISSED OFFSET
	float                                              NetCullDistanceSquared;                                   // 0x0144(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	int                                                NetTag;                                                   // 0x0148(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
	unsigned char                                      UnknownData11[0x4];                                       // 0x014C(0x0004) MISSED OFFSET
	float                                              NetUpdateFrequency;                                       // 0x0150(0x0004) (Edit, BlueprintVisible, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData12[0x4];                                       // 0x0154(0x0004) MISSED OFFSET
	float                                              MinNetUpdateFrequency;                                    // 0x0158(0x0004) (Edit, BlueprintVisible, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData13[0x4];                                       // 0x015C(0x0004) MISSED OFFSET
	float                                              NetPriority;                                              // 0x0160(0x0004) (Edit, BlueprintVisible, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
	unsigned char                                      UnknownData14[0x4];                                       // 0x0164(0x0004) MISSED OFFSET
	unsigned char                                      bAutoDestroyWhenFinished : 1;                             // 0x0168(0x0001) (BlueprintVisible)
	unsigned char                                      bCanBeDamaged : 1;                                        // 0x0168(0x0001) (Edit, BlueprintVisible, Net, SaveGame)
	unsigned char                                      bActorIsBeingDestroyed : 1;                               // 0x0168(0x0001) (Transient, DuplicateTransient)
	unsigned char                                      bCollideWhenPlacing : 1;                                  // 0x0168(0x0001)
	unsigned char                                      bFindCameraComponentWhenViewTarget : 1;                   // 0x0168(0x0001) (Edit, BlueprintVisible)
	unsigned char                                      bRelevantForNetworkReplays : 1;                           // 0x0168(0x0001)
	unsigned char                                      bGenerateOverlapEventsDuringLevelStreaming : 1;           // 0x0168(0x0001) (Edit, BlueprintVisible)
	unsigned char                                      bCanBeInCluster : 1;                                      // 0x0168(0x0001) (Edit)
	unsigned char b;                      // 0x0169(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData15[0x6];                                       // 0x016A(0x0006) MISSED OFFSET
	class APawn* Instigator;                                               // 0x0170(0x0008) (BlueprintVisible, Net, ZeroConstructor, IsPlainOldData)
	TArray<class AActor*>                              Children;                                                 // 0x0178(0x0010) (ZeroConstructor, Transient)
	class USceneComponent* RootComponent;                                            // 0x0188(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	TArray<class AMatineeActor*>                       ControllingMatineeActors;                                 // 0x0190(0x0010) (ZeroConstructor, Transient)
	float                                              InitialLifeSpan;                                          // 0x01A0(0x0004) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData16[0xC];                                       // 0x01A4(0x000C) MISSED OFFSET
	TArray<struct FName>                               Layers;                                                   // 0x01B0(0x0010) (ZeroConstructor)                          // 0x01C8(0x0008) (ExportObject, ZeroConstructor, InstancedReference, IsPlainOldData)
	unsigned char                                      bAllowReceiveTickEventOnDedicatedServer : 1;              // 0x01D0(0x0001)
	unsigned char                                      UnknownData17 : 3;                                        // 0x01D0(0x0001)
	unsigned char                                      bActorSeamlessTraveled : 1;                               // 0x01D0(0x0001)
	unsigned char                                      bIgnoresOriginShifting : 1;                               // 0x01D0(0x0001) (Edit)
	unsigned char                                      bEnableAutoLODGeneration : 1;                             // 0x01D0(0x0001) (Edit)
	unsigned char                                      UnknownData18[0x7];                                       // 0x01D1(0x0007) MISSED OFFSET
	TArray<struct FName>                               Tags;                                                     // 0x01D8(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	uint64_t                                           HiddenEditorViews;                                        // 0x01E8(0x0008) (ZeroConstructor, Transient, IsPlainOldData)
	struct FScriptMulticastDelegate                    OnTakeAnyDamage;                                          // 0x01F0(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnTakePointDamage;                                        // 0x0200(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnActorBeginOverlap;                                      // 0x0210(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnActorEndOverlap;                                        // 0x0220(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnBeginCursorOver;                                        // 0x0230(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnEndCursorOver;                                          // 0x0240(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnClicked;                                                // 0x0250(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnReleased;                                               // 0x0260(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnInputTouchBegin;                                        // 0x0270(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnInputTouchEnd;                                          // 0x0280(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnInputTouchEnter;                                        // 0x0290(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnInputTouchLeave;                                        // 0x02A0(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnActorHit;                                               // 0x02B0(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnDestroyed;                                              // 0x02C0(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	struct FScriptMulticastDelegate                    OnEndPlay;                                                // 0x02D0(0x0010) (ZeroConstructor, InstancedReference, BlueprintAssignable)
	unsigned char                                      UnknownData19[0xA8];                                      // 0x02E0(0x00A8) MISSED OFFSET
	TArray<class UActorComponent*>                     BlueprintCreatedComponents;                               // 0x0388(0x0010) (ExportObject, ZeroConstructor)
	TArray<class UActorComponent*>                     InstanceComponents;                                       // 0x0398(0x0010) (ExportObject, ZeroConstructor)
	unsigned char                                      UnknownData20[0x8];                                       // 0x03A8(0x0008) MISSED OFFSET
	unsigned char                                      UnknownData066[0xFFF];                                       // 0x0084(0x0004) MISSED OFFSET
};

struct AimInfo {
	uint64_t pMesh;
	uint64_t RootComponent;
	uint64_t Actor;
	BoneVisibleResult BoneState;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

enum Bones : int
{
	Root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	face_root = 7,
	eyebrows_pos_root = 8,
	eyebrows_root = 9,
	eyebrows_r = 10,
	eyebrows_l = 11,
	eyebrow_l = 12,
	eyebrow_r = 13,
	forehead_root = 14,
	forehead = 15,
	jaw_pos_root = 16,
	jaw_root = 17,
	jaw = 18,
	mouth_down_pos_root = 19,
	mouth_down_root = 20,
	lip_bm_01 = 21,
	lip_bm_02 = 22,
	lip_br = 23,
	lip_bl = 24,
	jaw_01 = 25,
	jaw_02 = 26,
	cheek_pos_root = 27,
	cheek_root = 28,
	cheek_r = 29,
	cheek_l = 30,
	nose_side_root = 31,
	nose_side_r_01 = 32,
	nose_side_r_02 = 33,
	nose_side_l_01 = 34,
	nose_side_l_02 = 35,
	eye_pos_r_root = 36,
	eye_r_root = 37,
	eye_rot_r_root = 38,
	eye_lid_u_r = 39,
	eye_r = 40,
	eye_lid_b_r = 41,
	eye_pos_l_root = 42,
	eye_l_root = 43,
	eye_rot_l_root = 44,
	eye_lid_u_l = 45,
	eye_l = 46,
	eye_lid_b_l = 47,
	nose_pos_root = 48,
	nose = 49,
	mouth_up_pos_root = 50,
	mouth_up_root = 51,
	lip_ul = 52,
	lip_um_01 = 53,
	lip_um_02 = 54,
	lip_ur = 55,
	lip_l = 56,
	lip_r = 57,
	hair_root = 58,
	hair_b_01 = 59,
	hair_b_02 = 60,
	hair_l_01 = 61,
	hair_l_02 = 62,
	hair_r_01 = 63,
	hair_r_02 = 64,
	hair_f_02 = 65,
	hair_f_01 = 66,
	hair_b_pt_01 = 67,
	hair_b_pt_02 = 68,
	hair_b_pt_03 = 69,
	hair_b_pt_04 = 70,
	hair_b_pt_05 = 71,
	camera_fpp = 72,
	GunReferencePoint = 73,
	GunRef = 74,
	breast_l = 75,
	breast_r = 76,
	clavicle_l = 77,
	upperarm_l = 113,
	lowerarm_l = 89,
	hand_l = 95,
	thumb_01_l = 81,
	thumb_02_l = 82,
	thumb_03_l = 83,
	thumb_04_l_MBONLY = 84,
	index_01_l = 85,
	index_02_l = 86,
	index_03_l = 87,
	index_04_l_MBONLY = 88,
	middle_01_l = 89,
	middle_02_l = 90,
	middle_03_l = 91,
	middle_04_l_MBONLY = 92,
	ring_01_l = 93,
	ring_02_l = 94,
	ring_03_l = 95,
	ring_04_l_MBONLY = 96,
	pinky_01_l = 97,
	pinky_02_l = 98,
	pinky_03_l = 99,
	pinky_04_l_MBONLY = 100,
	item_l = 101,
	lowerarm_twist_01_l = 102,
	upperarm_twist_01_l = 103,
	clavicle_r = 104,
	upperarm_r = 140,
	lowerarm_r = 116,
	hand_r = 126,
	thumb_01_r = 108,
	thumb_02_r = 109,
	thumb_03_r = 110,
	thumb_04_r_MBONLY = 111,
	index_01_r = 112,
	index_02_r = 113,
	index_03_r = 114,
	index_04_r_MBONLY = 115,
	middle_01_r = 116,
	middle_02_r = 117,
	middle_03_r = 118,
	middle_04_r_MBONLY = 119,
	ring_01_r = 120,
	ring_02_r = 121,
	ring_03_r = 122,
	ring_04_r_MBONLY = 123,
	pinky_01_r = 124,
	pinky_02_r = 125,
	pinky_03_r = 126,
	pinky_04_r_MBONLY = 127,
	item_r = 128,
	lowerarm_twist_01_r = 129,
	upperarm_twist_01_r = 130,
	BackPack = 131,
	backpack_01 = 132,
	backpack_02 = 133,
	Slot_Primary = 134,
	Slot_Secondary = 135,
	Slot_Melee = 136,
	slot_throwable = 137,
	coat_l_01 = 138,
	coat_l_02 = 139,
	coat_l_03 = 140,
	coat_l_04 = 141,
	coat_fl_01 = 142,
	coat_fl_02 = 143,
	coat_fl_03 = 144,
	coat_fl_04 = 145,
	coat_b_01 = 146,
	coat_b_02 = 147,
	coat_b_03 = 148,
	coat_b_04 = 149,
	coat_r_01 = 150,
	coat_r_02 = 151,
	coat_r_03 = 152,
	coat_r_04 = 153,
	coat_fr_01 = 154,
	coat_fr_02 = 155,
	coat_fr_03 = 156,
	coat_fr_04 = 157,
	thigh_l = 168,
	calf_l = 169,
	foot_l = 170,
	ball_l = 161,
	calf_twist_01_l = 162,
	thigh_twist_01_l = 163,
	thigh_r = 174,
	calf_r = 175,
	foot_r = 176,
	ball_r = 167,
	calf_twist_01_r = 168,
	thigh_twist_01_r = 169,
	Slot_SideArm = 170,
	skirt_l_01 = 171,
	skirt_l_02 = 172,
	skirt_l_03 = 173,
	skirt_f_01 = 174,
	skirt_f_02 = 175,
	skirt_f_03 = 176,
	skirt_b_01 = 177,
	skirt_b_02 = 178,
	skirt_b_03 = 179,
	skirt_r_01 = 180,
	skirt_r_02 = 181,
	skirt_r_03 = 182,
	ik_hand_root = 183,
	ik_hand_gun = 184,
	ik_hand_r = 185,
	ik_hand_l = 186,
	ik_aim_root = 187,
	ik_aim_l = 188,
	ik_aim_r = 189,
	ik_foot_root = 190,
	ik_foot_l = 191,
	ik_foot_r = 192,
	camera_tpp = 193,
	ik_target_root = 194,
	ik_target_l = 195,
	ik_target_r = 196,
	VB_spine_03_spine_03 = 197,
	VB_upperarm_r_lowerarm_r = 198
};
