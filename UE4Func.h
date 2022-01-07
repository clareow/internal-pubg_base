#pragma once
#include <Windows.h>
#include "offset.h"
#include "defines.h"

extern FHitResult OutHit;

//typedef bool (*_STATIC_LineTraceSingle)(
//	LPBYTE WorldContextObject, 
//	Vector3 Start, 
//	Vector3 End,
//	TEnumAsByte<ETraceTypeQuery> TraceChannel, 
//	bool bTraceComplex, 
//	TArray<ULONG_PTR> ActorsToIgnore, 
//	TEnumAsByte<EDrawDebugTrace> DrawDebugType, 
//	FHitResult& res, 
//	bool bIgnoreSelf);
//extern _STATIC_LineTraceSingle pCallTraceSingle;
//
//typedef bool(__fastcall* LineTraceSingle)
//(
//	const void* WorldContextObject,
//	const Vector3 Start,
//	const Vector3 End,
//	ETraceTypeQuery TraceChannel,
//	bool bTraceComplex,
//	const TArray < AActor* >& ActorsToIgnore,
//	EDrawDebugTrace DrawDebugType,
//	FHitResult& OutHit,
//	bool bIgnoreSelf,
//	FLinearColor TraceColor,
//	FLinearColor TraceHitColor,
//	float DrawTime
//	);

typedef bool(__fastcall* fnLineTraceSingle)(
	void*, // UWorld,
	const Vector3& Start,
	const Vector3& End,
	TEnumAsByte<ETraceTypeQuery>,
	bool bTraceComplex,
	const TArray <AActor>,
	TEnumAsByte<EDrawDebugTrace>,
	FHitResult* OutHit,
	bool bIgnoreSelf,
	const FLinearColor&,
	const FLinearColor&,
	float DrawTime
	);
extern fnLineTraceSingle pfnLineTraceSingle;

typedef void(__fastcall* fnSetRenderCustomDepth)(
	__int64 a1, unsigned __int8 a2
	);

bool GetVisBone(Vector3 Local, Vector3 Target);