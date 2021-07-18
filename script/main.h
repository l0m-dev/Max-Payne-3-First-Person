#pragma once

#include "types.h"

#define DLLIMPORT __declspec(dllimport)

DLLIMPORT void scriptRegister(HMODULE module, void(*function)());
DLLIMPORT void scriptUnregister(HMODULE module);
DLLIMPORT void scriptWait(DWORD ms);

DLLIMPORT void nativeInit(Hash hash);
DLLIMPORT void nativePush32(uint32_t value);
DLLIMPORT uint32_t* nativeCall();

DLLIMPORT uint32_t* getGlobalPtr(int globalId);

DLLIMPORT uint8_t* getBlipAddress(Blip blip);
DLLIMPORT uint8_t* getInteriorAddress(Interior interior);
DLLIMPORT uint8_t* getObjectAddress(Object object);
DLLIMPORT uint8_t* getPedAddress(Ped ped);
DLLIMPORT uint8_t* getVehicleAddress(Vehicle vehicle);

DLLIMPORT int worldGetAllBlips(Blip* arr, int arrSize);
DLLIMPORT int worldGetAllInteriors(Interior* arr, int arrSize);
DLLIMPORT int worldGetAllObjects(Object* arr, int arrSize);
DLLIMPORT int worldGetAllPeds(Ped* arr, int arrSize);
DLLIMPORT int worldGetAllVehicles(Vehicle* arr, int arrSize);

typedef void(*KeyboardHandler)(DWORD, WORD, BYTE, BOOL, BOOL, BOOL, BOOL);

DLLIMPORT void keyboardHandlerRegister(KeyboardHandler handler);
DLLIMPORT void keyboardHandlerUnregister(KeyboardHandler handler);

static void TERMINATE()
{
	scriptWait(INFINITE);
}