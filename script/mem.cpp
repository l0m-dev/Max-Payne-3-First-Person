#include "stdafx.h"
#include "mem.h"

void mem::PatchEx(void* dst, void* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void mem::NopEx(void* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

void mem::Patch(void* dst, void* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy (dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void mem::Nop(void* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

bool mem::Hook(void* src, void* dst, int len)
{
	if (len < 5) return false;

	DWORD  curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(src, 0x90, len);
	uintptr_t  relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;

	*(uintptr_t*)src = 0xE9;
	*(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

char* mem::TrampHook32(void* src, void* dst, const intptr_t len)
{
	// Make sure the length is greater than 5
	if (len < 5) return 0;

	// Create the gateway (len + 5 for the overwritten bytes + the jmp)
	void* gateway = VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//Write the stolen bytes into the gateway
	memcpy(gateway, src, len);

	// Get the gateway to destination addy
	intptr_t  gatewayRelativeAddr = ((intptr_t)src - (intptr_t)gateway) - 5;

	// Add the jmp opcode to the end of the gateway
	*(char*)((intptr_t)gateway + len) = 0xE9;

	// Add the address to the jmp
	*(intptr_t*)((intptr_t)gateway + len + 1) = gatewayRelativeAddr;

	// Perform the detour
	Hook(src, dst, len);

	return (char*)gateway;
}

void mem::ParsePattern(const char* combo, char* pattern, char* mask) {
	unsigned int patternLen = (strlen(combo) + 1) / 3;

	for (unsigned int i = 0; i < strlen(combo); i++) {
		if (combo[i] == ' ') {
			continue;
		}
		else if (combo[i] == '?') {
			mask[(i + 1) / 3] = '?';
			i += 2;
		}
		else {
			char byte = (char)strtol(&combo[i], 0, 16);
			pattern[(i + 1) / 3] = byte;
			mask[(i + 1) / 3] = 'x';
			i += 2;
		}
	}
	pattern[patternLen] = '\0';
	mask[patternLen] = '\0';
}

char* mem::ScanIn(const char* comboPattern, char* begin) {//, unsigned int size)
	unsigned int comboPatternLen = ((strlen(comboPattern) + 1) / 3) + 1;
	char* pattern = new char[comboPatternLen];
	char* mask = new char[comboPatternLen];

	ParsePattern(comboPattern, pattern, mask);

	unsigned int patternLength = strlen(mask);

	MODULEINFO modinfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)begin, &modinfo, sizeof(MODULEINFO));

	unsigned int size = modinfo.SizeOfImage;

	bool found = true;
	unsigned int i = 0;

	for (i; i < size - patternLength; i++) {
		found = true;
		for (unsigned int j = 0; j < patternLength; j++) {
			if (mask[j] != '?' && pattern[j] != *(begin + i + j)) {
				found = false;
				break;
			}
		}
		if (found) {
			break;
		}
	}

	delete[] pattern;
	delete[] mask;

	if (found)
		return (begin + i);
	else
		return nullptr;
}