#include "stdafx.h"
#include "global.h"

void ScriptMain();

#ifdef CONSOLE_ENABLED
extern FILE* f;
#endif

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//DisableThreadLibraryCalls(hModule);
		scriptRegister(hModule, ScriptMain);
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
#ifdef CONSOLE_ENABLED
		fclose(f);
		FreeConsole();
#endif
		PatchGameplayCameraPosition(ThirdPerson);
		PatchTurnMode(ThirdPerson);
		scriptUnregister(hModule);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}