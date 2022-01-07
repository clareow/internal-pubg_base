#include "UE4Func.h"
#include "defines.h"
#include "UGameMemory.h"

FHitResult OutHit;

fnLineTraceSingle pfnLineTraceSingle = reinterpret_cast<fnLineTraceSingle>((uint64_t)GetModuleHandle(0) + LTSoffset);

TArray<AActor> GetActors()
{
	__try
	{
		TArray<AActor> AActors;
		AActors = *(TArray<AActor>*)(UGameData->GetActors());
		//AActors = mem->RPM<TArray<AActor>>(Actor);
		return AActors;
	}
	__except (1) {}
}

bool LineTraceSingle(const Vector3& Start, const Vector3& End, ETraceTypeQuery TraceChannel, bool bTraceComplex, EDrawDebugTrace DrawDebugType, bool bIgnoreSelf, const FLinearColor& TraceColor, const FLinearColor& TraceHitColor, float DrawTime, FHitResult* pOutHit)
{
	__try
	{
		if (pOutHit)
			OutHit = *pOutHit;

		return !pfnLineTraceSingle((void*)UGameData->GetUWorld(), Start, End, TraceChannel, bTraceComplex, GetActors(), DrawDebugType, &OutHit, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
	}
	__except (1)
	{
		return 0;
	}
}

bool GetVisBone(Vector3 Local, Vector3 Target)
{
	return LineTraceSingle(Local, Target, ETraceTypeQuery::TraceTypeQuery1, true, EDrawDebugTrace::EDrawDebugTrace__None, true, FLinearColor(), FLinearColor(), 0.f, NULL);
}