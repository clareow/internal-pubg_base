#include "Decrypt.h"
#include "offset.h"

typedef __int64(__fastcall* Xenuine_Decrypt_)(__int64 Seed, __int64 Value);
Xenuine_Decrypt_ Xenuine_Decrypta = (Xenuine_Decrypt_)(*(uintptr_t*)((uintptr_t)GetModuleHandle(NULL) + decrypt));

uint64_t Decrypt(uint64_t Value) {
	if (!Value) {
		return 0;
	}
	return Xenuine_Decrypta(0x0483EC531, Value);
}

DWORD Decrypt_CIndex(DWORD value)
{
	DWORD v10 = _rotl(value ^ 0xD9E9D0CB, 6);
	DWORD result = v10 ^ (v10 << 0x10) ^ 0xA8EA01B4;
	return result;
}