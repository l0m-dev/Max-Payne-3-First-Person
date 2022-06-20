// copied from ScriptHookV SDK

#include "keyboard.h"

const int KEYS_SIZE = 255;
const int repeatDelay = 8;

struct {
	DWORD time;
	BOOL isWithAlt;
	BOOL wasDownBefore;
	BOOL isUpNow;
	BOOL isJustDownNow;
	DWORD pressedTime;
	int holdDelay;
} keyStates[KEYS_SIZE];

void OnKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	if (key < KEYS_SIZE)
	{
		keyStates[key].time = GetTickCount();
		keyStates[key].isWithAlt = isWithAlt;
		keyStates[key].wasDownBefore = wasDownBefore;
		keyStates[key].isUpNow = isUpNow;
		keyStates[key].isJustDownNow = !wasDownBefore;
		if (keyStates[key].isJustDownNow) {
			keyStates[key].pressedTime = GetTickCount();
			keyStates[key].holdDelay = repeatDelay;
		}
	}
}

const int NOW_PERIOD = 30, MAX_DOWN = 5000; // ms

bool IsKeyDown(DWORD key)
{
	return (key < KEYS_SIZE) ? ((GetTickCount() < keyStates[key].time + MAX_DOWN) && !keyStates[key].isUpNow) : false;
}

bool IsKeyJustUp(DWORD key, bool exclusive)
{
	bool b = (key < KEYS_SIZE) ? (GetTickCount() < keyStates[key].time + NOW_PERIOD && keyStates[key].isUpNow) : false;
	if (b && exclusive)
		ResetKeyState(key);
	return b;
}

bool IsKeyJustDown(DWORD key, bool exclusive) {
	bool b = (key < KEYS_SIZE) ? (GetTickCount() < keyStates[key].pressedTime + NOW_PERIOD && !keyStates[key].isUpNow) : false;
	if (b && exclusive)
		ResetKeyState(key);
	return b;
}

bool IsKeyRepeating(DWORD key, bool exclusive) {
	if (IsKeyJustDown(key))
		return true;

	bool b = IsKeyDown(key) ? (GetTickCount() > keyStates[key].pressedTime + 500 && keyStates[key].holdDelay <= 0) : false;
	keyStates[key].holdDelay--;
	if (b)
		keyStates[key].holdDelay = repeatDelay;

	return b;
}

void ResetKeyState(DWORD key)
{
	if (key < KEYS_SIZE)
		memset(&keyStates[key], 0, sizeof(keyStates[0]));
}