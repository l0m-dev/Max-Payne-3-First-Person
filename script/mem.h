#pragma once
#include <vector>
#include "stdafx.h"
#include <windows.h>

namespace mem
{
	void PatchEx(void* dst, void* src, unsigned int size, HANDLE hProcess);
	void NopEx(void* dst, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	void Patch(void* dst, void* src, unsigned int size);
	void Nop(void* dst, unsigned int size);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);

	bool Hook(void* src, void* dst, int len);
	char* TrampHook32(void* src, void* dst, const intptr_t len);

	void ParsePattern(const char* combo, char* pattern, char* mask);
	char* ScanIn(const char* comboPattern, char* begin);
}