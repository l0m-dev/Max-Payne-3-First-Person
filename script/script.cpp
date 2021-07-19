#include "stdafx.h"
#include "main.h"
#include "global.h"

#include "script.h"
#include <Windows.h>
#include <Math.h>

#include "mem.h"
#include "proc.h"

struct CameraInfo {
	float x;
	float y;
	float z;
	float d;
} gameplayCamera;

ViewMode currentViewMode = ThirdPerson;
ViewMode desiredViewMode = FirstPerson;
Camera firstPersonCamera;
CameraInfo* cameraAddr;
uintptr_t cameraOffset;

bool firstPersonCover = false;
bool hideHead = true;

uintptr_t moduleBase = (uintptr_t)GetModuleHandle(nullptr);
BYTE *nopAddr, *nopAddr1, *nopAddr2, *nopAddr3, *nopAddr4;

Vector3 lerpedPos = Vector3::zero();
//Vector3 coverLerpPos = Vector3::zero();
int interpolateStart = -1;
bool interpolatingCamera = false;
int interpTime = 300;

int lastShotTime = 0;
int lastLevelIndex = -1;
bool disableGameCameraSwitching = false;

int defaultTextureVariation = 0, defaultTextureVariation8 = 0, defaultTextureVariation12 = 0, defaultDrawableVariation8 = 0, defaultDrawableVariation12 = 0;

void PatchFirstPerson() {
	const char* comboPattern = "24 40 30 86 95 01 00 00";
	const char* comboPattern2 = "75 ?? 6A 00 6A 01 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 85 C0";

	BYTE* addr = (BYTE*)(mem::ScanIn(comboPattern, (char*)moduleBase));
	BYTE* addr2 = (BYTE*)(mem::ScanIn(comboPattern2, (char*)moduleBase));

	if (addr)
		mem::Nop(addr + 0x2, 6); // fixes not being able to shoot

	if (addr2)
		mem::Nop(addr2, 2); // fixes movement
}

void PatchGameplayCameraPosition(ViewMode viewMode) {
	BYTE nopBytes[30] = { 0xD9, 0x00, 0xF3, 0x0F, 0x10, 0x40, 0x04, 0xF3, 0x0F, 0x10, 0x48, 0x08, 0xD9, 0x1E, 0xF3, 0x0F, 0x11, 0x46, 0x04, 0xF3, 0x0F,
		0x11, 0x4E, 0x08, 0xD9, 0x40, 0x0C, 0xD9, 0x5E, 0x0C };
	BYTE nop1Bytes_5[25] = { 0xF3, 0x0F, 0x11, 0x00, 0xF3, 0x0F, 0x10, 0x44, 0x24, 0x1C, 0xF3, 0x0F, 0x11, 0x48, 0x04, 0xF3, 0x0F, 0x11, 0x50, 0x08, 0xF3,
		0x0F, 0x11, 0x40, 0x0C };

	BYTE nop2Bytes_4[27] = { 0xF3, 0x0F, 0x11, 0x00, 0xF3, 0x0F, 0x11, 0x48, 0x04, 0xF3, 0x0F, 0x11, 0x58, 0x0C, 0xF3, 0x0F, 0x58, 0xD5, 0xF3, 0x0F, 0x58,
		0xD4, 0xF3, 0x0F, 0x11, 0x50, 0x08 };
	BYTE nop2Bytes_28[4] = { 0xF3, 0x0F, 0x11, 0x18 };
	BYTE nop2Bytes_35[5] = { 0xF3, 0x0F, 0x11, 0x40, 0x04 };
	BYTE nop2Bytes_43[5] = { 0xF3, 0x0F, 0x11, 0x40, 0x08 };

	//BYTE nop3Bytes[8] = { 0xF3, 0x0F, 0x11, 0x9E, 0x5C, 0x02, 0x00, 0x00 };
	//BYTE nop3Bytes_152[8] = { 0xF3, 0x0F, 0x11, 0x8E, 0x5C, 0x02, 0x00, 0x00 };
	//BYTE nop3Bytes_1D1[8] = { 0xF3, 0x0F, 0x11, 0x96, 0x5C, 0x02, 0x00, 0x00 };

	BYTE nop4Bytes[6] = { 0x88, 0x9E, 0xA8, 0x00, 0x00, 0x00 };

	if (viewMode == ThirdPerson) {
		if (nopAddr)
			mem::Patch((BYTE*)nopAddr, nopBytes, sizeof(nopBytes));
		if (nopAddr1)
			mem::Patch((BYTE*)(nopAddr1), nop1Bytes_5, sizeof(nop1Bytes_5));

		if (nopAddr1) { // not typo
			mem::Patch((BYTE*)(nopAddr2 + 0x4), nop2Bytes_4, sizeof(nop2Bytes_4));
			mem::Patch((BYTE*)(nopAddr2 + 0x28), nop2Bytes_28, sizeof(nop2Bytes_28));
			mem::Patch((BYTE*)(nopAddr2 + 0x35), nop2Bytes_35, sizeof(nop2Bytes_35));
			mem::Patch((BYTE*)(nopAddr2 + 0x43), nop2Bytes_43, sizeof(nop2Bytes_43));
		}

		// for shootdodge
		if (nopAddr3) {
			//mem::Patch((BYTE*)nopAddr3, nop3Bytes, sizeof(nop3Bytes));
			//mem::Patch((BYTE*)(nopAddr3 + 0x152), nop3Bytes_152, sizeof(nop3Bytes_152));
			//mem::Patch((BYTE*)(nopAddr3 + 0x1D1), nop3Bytes_1D1, sizeof(nop3Bytes_1D1));
		}
	} else {
		if (nopAddr)
			mem::Nop((BYTE*)nopAddr, sizeof(nopBytes));
		if (nopAddr1)
			mem::Nop((BYTE*)(nopAddr1), sizeof(nop1Bytes_5));

		if (nopAddr1) {
			mem::Nop((BYTE*)(nopAddr2 + 0x4), sizeof(nop2Bytes_4));
			mem::Nop((BYTE*)(nopAddr2 + 0x28), sizeof(nop2Bytes_28));
			mem::Nop((BYTE*)(nopAddr2 + 0x35), sizeof(nop2Bytes_35));
			mem::Nop((BYTE*)(nopAddr2 + 0x43), sizeof(nop2Bytes_43));
		}

		// for shootdodge
		if (nopAddr3) {
			//mem::Nop((BYTE*)nopAddr3, sizeof(nop3Bytes));
			//mem::Nop((BYTE*)(nopAddr3 + 0x152), sizeof(nop3Bytes_152));
			//mem::Nop((BYTE*)(nopAddr3 + 0x1D1), sizeof(nop3Bytes_1D1));
		}
	}
}

void DisableGameCameraSwitching(bool disable) {
	BYTE nop4Bytes[6] = { 0x88, 0x9E, 0xA8, 0x00, 0x00, 0x00 };

	disableGameCameraSwitching = disable;

	if (disable) {
		if (nopAddr4) {
			mem::Nop((BYTE*)nopAddr4, 6);
		}
	} else {
		if (nopAddr4) {
			mem::Patch((BYTE*)nopAddr4, nop4Bytes, sizeof(nop4Bytes));
		}
	}
}

void GetAddresses() {
	const char* camComboPattern = "F0 41 CD CC 4C 3D CD CC 4C 3D 66 66 66 3F 35 FA 8E 3C 00 00 00 00 00 00 00 00";
	cameraOffset = (uintptr_t)(mem::ScanIn(camComboPattern, (char*)moduleBase) + 0x1e);

	const char* comboPattern = "D9 00 F3 0F 10 40 04 F3 0F 10 48 08 D9 1E F3 0F 11 46 04 F3 0F 11 4E 08 D9 40 0C D9 5E 0C 8B 43 34 85 C0 0F";
	nopAddr = (BYTE*)(mem::ScanIn(comboPattern, (char*)moduleBase));

	const char* comboPattern1 = "F3 0F 11 00 F3 0F 10 44 24 1C F3 0F 11 48 04 F3 0F 11 50 08 F3 0F 11 40 0C 5E 8B E5 5D C2 0C 00 CC";
	nopAddr1 = (BYTE*)(mem::ScanIn(comboPattern1, (char*)moduleBase));

	nopAddr2 = nopAddr1 - 0xCC;

	const char* comboPattern3 = "F3 0F 11 9E 5C 02 00 00 8B 87 64 15 00 00 85 C0";
	nopAddr3 = (BYTE*)(mem::ScanIn(comboPattern3, (char*)moduleBase));

	const char* comboPattern4 = "88 9E A8 00 00 00 5F 5E 5B C2 10 00";
	nopAddr4 = (BYTE*)(mem::ScanIn(comboPattern4, (char*)moduleBase));
}

void SetGameplayCameraLocation(float x, float y, float z, float d) {
	gameplayCamera.x = x;
	gameplayCamera.y = y;
	gameplayCamera.z = z;
	gameplayCamera.d = d;

	*cameraAddr = gameplayCamera;
	*(float*)(cameraAddr + 10) = gameplayCamera.d;
}

void ShowPlayerHead(bool show) {
	if (!hideHead)
		return;

	Ped plrPed = PLAYER::GET_PLAYER_PED(PLAYER::GET_PLAYER_ID());

	PED::SET_PED_COMPONENT_VARIATION(plrPed, 0, PED::GET_PED_DRAWABLE_VARIATION(plrPed, 0), show ? defaultTextureVariation : 0, 0);
	PED::SET_PED_COMPONENT_VARIATION(plrPed, 8, PED::GET_PED_DRAWABLE_VARIATION(plrPed, 8), show ? defaultTextureVariation8 : 0, 0);
	PED::SET_PED_COMPONENT_VARIATION(plrPed, 12, PED::GET_PED_DRAWABLE_VARIATION(plrPed, 12), show ? defaultTextureVariation12 : 4, 0);
	PED::SET_PED_COMPONENT_VARIATION(plrPed, 8, show ? defaultDrawableVariation8 : 0, PED::GET_PED_TEXTURE_VARIATION(plrPed, 8), 0);
	PED::SET_PED_COMPONENT_VARIATION(plrPed, 12, show ? defaultDrawableVariation12 : 1, PED::GET_PED_TEXTURE_VARIATION(plrPed, 12), 0);
	scriptWait(1);

	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_HEAD, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_HAIR, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_TEEF, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_TASK, show);
}

bool IsInCutscene() {
	return CAM::IS_ANIMATED_CAMERA_PLAYING() || CUTSCENE::GET_CUTSCENE_TIME_MS() > 0;
}

bool FirstPersonAllowed(Player player) {
	Ped plrPed = PLAYER::GET_PLAYER_PED(player);

	if (!CAM::IS_SCREEN_FADED_IN())
		return false;

	if (!firstPersonCover && PED::IS_PED_IN_COVER(plrPed))
		return false;

	// this check is for some scripted events where you don't want to be in fp
	if (!IsInCutscene() && !PLAYER::IS_PLAYER_CONTROL_ON(player) && abs(CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING()) > 150)
		return false;

	int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();
	if (currentLevel == 21 && ISEQ::ISEQ_GET_STATE(-1709834613) == 3) // ch 14, cp 11
		return false;

	// fire part first stairs = -211988314 ?
	if (currentLevel == 12 && ISEQ::ISEQ_GET_STATE(1252772788) == 3) // ch 6, plank
		return false;

	return !IsInCutscene() && !CAM::IS_BULLET_CAMERA_ACTIVE() && !PLAYER::IS_PLAYER_DOING_MELEE_GRAPPLE(player) &&
	    !PLAYER::IS_PLAYER_DOING_MELEE_GRAPPLE(player) && !PED::IS_PED_SWIMMING(plrPed) && !HUD::IS_SNIPER_SCOPE_VISIBLE() &&
	    !HUD::IS_PAUSE_MENU_ACTIVE() && !PLAYER::IS_PLAYER_CLIMBING(player) && PED::IS_PED_VISIBLE(plrPed) && !PLAYER::IS_PLAYER_DEAD(player);
}

bool ShouldResetStreaming() {
	int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();

	if (currentLevel == 12 && (ISEQ::ISEQ_GET_STATE(1252772788) >= 2)) // ch 6, plank
		return false;

	return true;
}

void SetupViewMode(ViewMode viewMode, Vector3 cameraLocation, Vector3 cameraRotation, float cameraFOV, bool transition) {
	Vector3 gameplayCameraLocation = CAM::GET_GAMEPLAY_CAM_COORD();
	bool tpTransitionAllowed = !CAM::IS_BULLET_CAMERA_ACTIVE();

	if (viewMode == FirstPerson && (cameraLocation.IsZero() || gameplayCameraLocation.IsZero()))
		return;

	if (viewMode == ThirdPerson && HUD::IS_PAUSE_MENU_ACTIVE())
		return;

	currentViewMode = viewMode;
	PatchGameplayCameraPosition(viewMode);

	//printf("FP Camera: x: %f, Y: %f, Z: %F, Fov: %f\n", cameraLocation.x, cameraLocation.y, cameraLocation.z, cameraFOV);
	//printf("TP Camera: x: %f, Y: %f, Z: %F, Fov: %f\n", gameplayCameraLocation.x, gameplayCameraLocation.y, gameplayCameraLocation.z, cameraFOV);

	if (viewMode == FirstPerson) {
		PLAYER::SET_PLAYER_COMBAT_TIMER_MODE(5);

		lerpedPos = cameraLocation;
		SetGameplayCameraLocation(lerpedPos.x, lerpedPos.y, lerpedPos.z, -1.04719758f);

		if (transition) {
			interpolatingCamera = true;
			interpolateStart = MISC::GET_GAME_TIMER();
		} else {
			ShowPlayerHead(false);
		}

		//if (ShouldResetStreaming())
		//	STREAMING::RESET_STREAMING_POINT_OF_INTEREST();

		if (!CAM::DOES_CAM_EXIST(firstPersonCamera)) {
			firstPersonCamera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", cameraLocation.x, cameraLocation.y, cameraLocation.z,
			    cameraRotation.x, cameraRotation.y, cameraRotation.z, cameraFOV, 1);
		}

		CAM::SET_CAM_ACTIVE(firstPersonCamera, 1);
		CAM::RENDER_SCRIPT_CAMS(1, transition, interpTime, 0); // first bool - what camera to render, 1 = scripted camera, 0 = gameplay camera
	} else {
		PLAYER::SET_PLAYER_COMBAT_TIMER_MODE(1);
		ShowPlayerHead(true);

		CAM::RENDER_SCRIPT_CAMS(0, tpTransitionAllowed, interpTime, 0);
		//CAM::SET_CAMERA_OVERRIDE("", "", 0, 1, 1);
	}
}

int main() {
	char tempFloat[32];

	bool enabled = GetPrivateProfileIntA("SETTINGS", "ENABLED", 1, ".\\FirstPerson.ini") != 0;
	desiredViewMode = GetPrivateProfileIntA("SETTINGS", "FIRSTPERSON_DEFAULT_ENABLED", 1, ".\\FirstPerson.ini") != 0 ? FirstPerson : ThirdPerson;
	GetPrivateProfileStringA("SETTINGS", "FIRSTPERSON_FOV", "60.0", tempFloat, 32, ".\\FirstPerson.ini");
	float firstPersonFOV = strtof(tempFloat, nullptr);
	firstPersonCover = GetPrivateProfileIntA("SETTINGS", "FIRSTPERSON_COVER", 0, ".\\FirstPerson.ini");
	int changeViewModeKey = GetPrivateProfileIntA("SETTINGS", "CHANGE_VIEW_MODE_KEY", 0x56, ".\\FirstPerson.ini");
	bool hideHud = GetPrivateProfileIntA("SETTINGS", "HIDE_HUD", 1, ".\\FirstPerson.ini") != 0;
	bool preferTwoHandedWeaponAfterCutscene = GetPrivateProfileIntA("SETTINGS", "PREFER_TWO_HANDED_WEAPON_AFTER_CUTSCENE", 1, ".\\FirstPerson.ini") != 0;

	bool debug = GetPrivateProfileIntA("ADVANCED", "DEBUG", 0, ".\\FirstPerson.ini") != 0;
	hideHead = GetPrivateProfileIntA("ADVANCED", "HIDE_HEAD_FIRSTPERSON", 1, ".\\FirstPerson.ini") != 0;
	Vector3 offset = Vector3::zero();

	GetPrivateProfileStringA("ADVANCED", "OFFSET_X", "0.1", tempFloat, 32, ".\\FirstPerson.ini");
	offset.x = strtof(tempFloat, nullptr);

	GetPrivateProfileStringA("ADVANCED", "OFFSET_Y", "0.2", tempFloat, 32, ".\\FirstPerson.ini");
	offset.y = strtof(tempFloat, nullptr);

	GetPrivateProfileStringA("ADVANCED", "OFFSET_Z", "0.1", tempFloat, 32, ".\\FirstPerson.ini");
	offset.z = strtof(tempFloat, nullptr);

	bool wasCutscene = false;
	bool wasBulletCamera = false;
	int lastCutsceneTime = 0;

	float DesiredHeading = 0;

	if (!enabled)
		return 0;

	PatchFirstPerson();
	GetAddresses();

	while (true) {
		if (PLAYER::DOES_MAIN_PLAYER_EXIST()) {
			Player player = PLAYER::GET_PLAYER_ID();
			Ped plrPed = PLAYER::GET_PLAYER_PED(player);

			int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();

			if (currentLevel == 115) { // frontend
				scriptWait(100);
				continue;
			}

			if (HUD::IS_PAUSE_MENU_ACTIVE() && currentViewMode == FirstPerson && !interpolatingCamera) {
				HUD::SET_FRONTEND_ACTIVE(0);
				CAM::RENDER_SCRIPT_CAMS(0, 1, 3000, 0);
				scriptWait(0);
				HUD::SET_FRONTEND_ACTIVE(1);
				scriptWait(0);
			}

			//printf("Level index: %i\n", currentLevel);

			if (IsKeyJustUp(VK_F9)) {
				MISC::RETURN_TO_TITLESCREEN("RIP");
			}

			cameraAddr = (CameraInfo*)(*(uintptr_t*)cameraOffset + 0x170);

			Vector3 playerHeadPosition = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::HEAD, offset.x, offset.y, offset.z);
			Vector3 playerFootPosition = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::R_FOOT, 0, 0, 0);

			//Vector3 gameplayCameraLocation = CAM::GET_GAMEPLAY_CAM_COORD();
			Vector3 gameplayCameraRotation = CAM::GET_GAMEPLAY_CAM_ROT();
			Vector3 camrot = CAM::GET_CAM_ROT(firstPersonCamera);

			Vector3 pedPos = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::HEAD, 0.0f, 0.0f, 0.25f);
			Vector3 gunPos = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::R_HAND, 0.0f, 0.0f, 0.25f);

			float heading = PED::GET_PED_HEADING(plrPed);
			float camRelativeHeading = CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING();

			bool rolling = playerFootPosition.z > playerHeadPosition.z && !PED::IS_PED_IN_AIR(plrPed);

			if (debug) {
				char array[128];
				//snprintf(array, sizeof(array), "%i %i %i %i Speed: %f", (int)camRelativeHeading, (int)heading, CAM::IS_CAM_RENDERING(FirstPersonCamera), CAM::GET_RENDERING_CAM(), PED::GET_PED_SPEED(plrPed));
				//snprintf(array, sizeof(array), "x: %f Y: %f Z: %f", playerHeadPosition.x, playerHeadPosition.y, playerHeadPosition.z);
				//snprintf(array, sizeof(array), "Level: %i Seq1: %i Seq2: %i Seq3: %i", currentLevel, ISEQ::ISEQ_GET_STATE(1252772788), ISEQ::ISEQ_GET_STATE(1549233931), ISEQ::ISEQ_GET_STATE(736610621));
				snprintf(array, sizeof(array), "X: %f Y: %f Z: %f", offset.x, offset.y, offset.z);

				HUD::PRINT_STRING_WITH_LITERAL_STRING_NOW("STRING", array, 100, 1);

				if (IsKeyJustUp(VK_NUMPAD7))
					offset.x -= 0.1f;
				if (IsKeyJustUp(VK_NUMPAD9))
					offset.x += 0.1f;

				if (IsKeyJustUp(VK_NUMPAD4))
					offset.y -= 0.1f;
				if (IsKeyJustUp(VK_NUMPAD6))
					offset.y += 0.1f;

				if (IsKeyJustUp(VK_NUMPAD1))
					offset.z -= 0.1f;
				if (IsKeyJustUp(VK_NUMPAD3))
					offset.z += 0.1f;
			}

			if (IsKeyJustUp(changeViewModeKey)) {
				desiredViewMode = (ViewMode)!desiredViewMode;
			}

			if (interpolatingCamera) {
				if (MISC::GET_GAME_TIMER() > interpolateStart + interpTime) { // CAM::IS_CAM_INTERPOLATING(FirstPersonCamera) is not working
					interpolatingCamera = false;

					ShowPlayerHead(false);
				}
			}

			if (FirstPersonAllowed(player)) {
				if (currentViewMode != desiredViewMode && !interpolatingCamera) {
					SetupViewMode(desiredViewMode, playerHeadPosition, gameplayCameraRotation, firstPersonFOV, 1);

					if (currentLevel == 13 && !disableGameCameraSwitching && !WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0))
						DisableGameCameraSwitching(1);
				}
			} else {
				if (currentViewMode == FirstPerson)
					SetupViewMode(ThirdPerson);
			}

			// get times
			if (IsInCutscene())
				lastCutsceneTime = MISC::GET_GAME_TIMER();

			if (PED::IS_PED_SHOOTING(plrPed))
				lastShotTime = MISC::GET_GAME_TIMER();

			// first person rendering
			if (currentViewMode == FirstPerson) {
				if (!CAM::IS_CAM_RENDERING(firstPersonCamera)) {
					SetupViewMode(FirstPerson, playerHeadPosition, gameplayCameraRotation, firstPersonFOV, interpolatingCamera);
				}

				if (currentLevel == 14 && (int)playerHeadPosition.x == 22 && (int)playerHeadPosition.y == -258) // ch 7, cp 6
					STREAMING::RESET_STREAMING_POINT_OF_INTEREST();

				if (abs(camRelativeHeading) > 140 + 0) {
					DesiredHeading = (heading + camRelativeHeading);
				}

				if (DesiredHeading) {
					if (abs(camRelativeHeading) < 20) {
						DesiredHeading = 0;
					} else {
						if (DesiredHeading > heading)
							heading = heading + 1;
						else
							heading = heading - 1;

						Vector3 vec = PED::GET_PED_VELOCITY(plrPed);
						float velSq = vec.x * vec.x + vec.y * vec.y;

						if (currentViewMode == FirstPerson && !PED::IS_PED_DUCKING(plrPed) && (velSq < 5))
							PED::SET_PED_DESIRED_HEADING(plrPed, heading);
					}
				}

				if (lerpedPos.z == 0)
					lerpedPos = playerHeadPosition;

				if (PED::GET_PED_SPEED(plrPed) > 0 && PED::GET_PED_SPEED(plrPed) < 5 && !PED::IS_PED_IN_ANY_TRAIN(plrPed) && !rolling)
					lerpedPos = Vector3::lerp(lerpedPos, playerHeadPosition, 0.89f - MISC::GET_FRAME_TIME() * 10);
				else
					lerpedPos = playerHeadPosition;

				if (gameplayCameraRotation.x > 0) {
					lerpedPos.z += gameplayCameraRotation.x / 900;
				}

				if (rolling) {
					lerpedPos.x += 0.2f;
					lerpedPos.z += 0.2f;
				}

				if (gameplayCameraRotation.x > 45)
					CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(45, 1065353216, 0);

				CAM::SET_CAM_COORD(firstPersonCamera, lerpedPos.x, lerpedPos.y, lerpedPos.z);
				CAM::SET_CAM_ROT(firstPersonCamera, gameplayCameraRotation.x, gameplayCameraRotation.y, gameplayCameraRotation.z);

				if (cameraAddr && CAM::GET_RENDERING_CAM() != -1) {
					SetGameplayCameraLocation(lerpedPos.x, lerpedPos.y, lerpedPos.z, -1.04719758f);
					float TempCamRelativeHeading = camRelativeHeading;
					float limit = 80;

					if ((PED::IS_PED_IN_COVER(plrPed) && MISC::GET_GAME_TIMER() > lastShotTime + 200) ||
					    !WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0)) {
						if (TempCamRelativeHeading < -limit)
							TempCamRelativeHeading = -limit;
						else if (TempCamRelativeHeading > limit)
							TempCamRelativeHeading = limit;

						CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(TempCamRelativeHeading, 1065353216, 0);
					}
				}
			} else {
				interpolateStart = -1;
				interpolatingCamera = false;

				defaultTextureVariation = PED::GET_PED_TEXTURE_VARIATION(plrPed, 0);
				defaultTextureVariation8 = PED::GET_PED_TEXTURE_VARIATION(plrPed, 8);
				defaultTextureVariation12 = PED::GET_PED_TEXTURE_VARIATION(plrPed, 12);
				defaultDrawableVariation8 = PED::GET_PED_DRAWABLE_VARIATION(plrPed, 8);
				defaultDrawableVariation12 = PED::GET_PED_DRAWABLE_VARIATION(plrPed, 12);

				if (disableGameCameraSwitching)
					DisableGameCameraSwitching(0);
			}

			if (IsInCutscene() && !wasCutscene) {
				// just entered cutscene

				wasCutscene = true;
			}

			if (!IsInCutscene() && wasCutscene) {
				// just exited cutscene

				wasCutscene = false;

				if (preferTwoHandedWeaponAfterCutscene) {
					int wep = WEAPON::GET_WEAPON_FROM_HOLSTER(plrPed, 2);
					if (wep)
						WEAPON::SELECT_WEAPON_TO_HAND(plrPed, wep, 0, 0);
				}
			}

			if (hideHud) {
				HUD::ENABLE_PLAYERHEALTH(false);
				HUD::ENABLE_BULLETTIMEMETER(false);
				HUD::TOGGLE_DISPLAY_AMMO(false);
				HUD::ENABLE_WEAPONPICKUP(false);
			}

			//end if player exists
		}
		scriptWait(0);
	}

	return 0;
}

#ifdef CONSOLE_ENABLED
FILE* f;
#endif

void ScriptMain() {
	srand(GetTickCount());

#ifdef CONSOLE_ENABLED
	AllocConsole();

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	HWND ConsoleHwnd = GetConsoleWindow();
	int ConsoleWidth = 250;
	int ConsoleHeight = 300;

	SetWindowPos(
	    ConsoleHwnd, nullptr, desktop.right - ConsoleWidth, desktop.bottom - ConsoleHeight - 28, ConsoleWidth, ConsoleHeight, SWP_NOZORDER); //SWP_NOSIZE
	HANDLE hOutConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hInConsole = GetStdHandle(STD_INPUT_HANDLE);

	SetConsoleTextAttribute(hOutConsole, FOREGROUND_GREEN);

	freopen_s(&f, "CONIN$", "r", stdin);
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);

	std::cout << "Started\n";
#endif
	main();
}
