#include "natives.h"
#include "keyboard.h"
#include "global.h"

void ScriptMain();

#ifdef _WIN32
#define NULL_DEVICE "NUL:"
#else
#define NULL_DEVICE "/dev/null"
#endif

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		scriptRegister(hModule, ScriptMain);
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
#ifdef CONSOLE_ENABLED
		FILE* stream;
		freopen_s(&stream, NULL_DEVICE, "r", stdin);
		freopen_s(&stream, NULL_DEVICE, "w", stdout);
		freopen_s(&stream, NULL_DEVICE, "w", stderr);

		SetConsoleCtrlHandler(nullptr, FALSE);

		FreeConsole();
#endif
		Ped oneped;
		if (worldGetAllPeds(&oneped, 1) > 0) {
			// if we unloaded the game, don't call any natives
			SetupViewMode(ThirdPerson);
			PatchTurnMode(ThirdPerson);
		}

		scriptUnregister(hModule);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}