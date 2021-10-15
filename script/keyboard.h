#pragma once

#include <windows.h>

void OnKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);

bool IsKeyDown(DWORD key);
bool IsKeyJustUp(DWORD key, bool exclusive = true);
bool IsKeyJustDown(DWORD key, bool exclusive = true);
bool IsKeyRepeating(DWORD key, bool exclusive = true);
void ResetKeyState(DWORD key);