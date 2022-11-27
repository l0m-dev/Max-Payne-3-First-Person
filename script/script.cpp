#include "main.h"
#include "global.h"

#include "script.h"
#include "enums.h"

#include <Windows.h>
#include <math.h>

#include "mem.h"
#include "proc.h"

#include "menu.h"

#pragma warning(disable : 4244 4305) // double <-> float conversions

#define MIN_FOV 10
#define MAX_FOV 130

bool isTrainerVisible = false;
bool mainMenuActive = true;

bool hidePlayerDamage = false;
bool disableBlackBars = false;

struct CameraInfo {
	float x;
	float y;
	float z;
	//float d;
} gameplayCamera;

ViewMode currentViewMode = ThirdPerson;
ViewMode desiredViewMode = FirstPerson;
Camera firstPersonCamera;
CameraInfo* cameraAddr;
uintptr_t cameraOffset;

Vector3 offset = Vector3::zero();
Vector3 defaultOffset = { 0.1, 0.12, 0.1 };
float firstPersonFOV;
float firstPersonFOVReduction;
bool firstPersonCover = false;
bool preferTwoHandedWeaponAfterCutscene;
bool hideHead = true;
bool hideHud = false;
bool debug = false;

uintptr_t moduleBase = (uintptr_t)GetModuleHandle(nullptr);
BYTE *nopAddr, *nopAddr1, *nopAddr2, *nopAddr3, *nopAddr3_2, *nopAddr3_3, *nopAddr4, *combatModeAddr, *turnModeAddr, *aimingAddr, *aimingIKAddr,
    *instantFlickAddr, *blackBarsAddr;
BYTE* controllerFixAddr;

bool fpTurnMode = false;

Vector3 lerpedPos = Vector3::zero();
//Vector3 coverLerpPos = Vector3::zero();
float lerpedForwardScale = 0;
float lerpedFOV = 0;

int interpolateStart = -1;
bool interpolatingCamera = false;
int interpTime = 300;

int lastShotTime = 0;
int lastLevelIndex = -1;
bool disableGameCameraSwitching = false;
//bool disableAimingIK = false;
bool inLMSfromCover = false;
bool wasScoped = false;
bool wasInBulletCam = false;
bool wasInMelee = false;

struct Props {
	int hat;
	int glasses;
} PlayerProps;

double DegreesToRadians(double degree) {
	return degree * (3.141592653589793238463 / 180.0);
}

void PatchFirstPerson() {
	const char* comboPattern = "24 40 30 86 95 01 00 00";
	const char* comboPattern2 = "75 ?? 6A 00 6A 01 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 85 C0";
	const char* comboPattern3 = "74 ?? 80 b8 ?? ?? ?? ?? ?? c6 44 24 ?? ?? 75 ?? c6 44 24 ?? ?? 80 7c 24";

	BYTE* addr = (BYTE*)(mem::ScanIn(comboPattern, (char*)moduleBase));
	BYTE* addr2 = (BYTE*)(mem::ScanIn(comboPattern2, (char*)moduleBase));
	BYTE* addr3 = (BYTE*)(mem::ScanIn(comboPattern3, (char*)moduleBase));

	BYTE arcadeByte = 0xEB;

	if (addr)
		mem::Nop(addr + 0x2, 6); // fixes not being able to shoot

	if (addr2)
		mem::Nop(addr2, 2); // fixes movement

	if (addr3)
		mem::Patch(addr3, &arcadeByte, sizeof(arcadeByte)); // fixes HUD not showing in arcade modes
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

	BYTE nop3Bytes[8] = { 0xF3, 0x0F, 0x11, 0x9E, 0x5C, 0x02, 0x00, 0x00 };
	BYTE nop3_2Bytes[8] = { 0xF3, 0x0F, 0x11, 0x8E, 0x5C, 0x02, 0x00, 0x00 };
	BYTE nop3_3Bytes[8] = { 0xF3, 0x0F, 0x11, 0x96, 0x5C, 0x02, 0x00, 0x00 };

	BYTE nop4Bytes[6] = { 0x88, 0x9E, 0xA8, 0x00, 0x00, 0x00 };

	BYTE combatModeBytes[2] = { 0x75, 0x08 };

	BYTE instantFlickByte = 0x74;
	BYTE instantFlickByte_dis = 0xEB;

	if (viewMode == ThirdPerson) {
		if (nopAddr)
			mem::Patch(nopAddr, nopBytes, sizeof(nopBytes));
		if (nopAddr1)
			mem::Patch(nopAddr1, nop1Bytes_5, sizeof(nop1Bytes_5));

		if (nopAddr1) { // not typo
			mem::Patch(nopAddr2 + 0x4, nop2Bytes_4, sizeof(nop2Bytes_4));
			mem::Patch(nopAddr2 + 0x28, nop2Bytes_28, sizeof(nop2Bytes_28));
			mem::Patch(nopAddr2 + 0x35, nop2Bytes_35, sizeof(nop2Bytes_35));
			mem::Patch(nopAddr2 + 0x43, nop2Bytes_43, sizeof(nop2Bytes_43));
		}

		//if (nopAddr3)
		//	mem::Patch((BYTE*)nopAddr3, nop3Bytes, sizeof(nop3Bytes));

		if (nopAddr3_2)
			mem::Patch(nopAddr3_2, nop3_2Bytes, sizeof(nop3_2Bytes));

		if (nopAddr3_3)
			mem::Patch(nopAddr3_3, nop3_3Bytes, sizeof(nop3_3Bytes));

		if (combatModeAddr)
			mem::Patch(combatModeAddr + 0x7, combatModeBytes, sizeof(combatModeBytes));

		if (instantFlickAddr)
			mem::Patch(instantFlickAddr + 0x7, &instantFlickByte, sizeof(instantFlickByte));
	} else {
		if (nopAddr)
			mem::Nop(nopAddr, sizeof(nopBytes));
		if (nopAddr1)
			mem::Nop(nopAddr1, sizeof(nop1Bytes_5));

		if (nopAddr1) {
			mem::Nop(nopAddr2 + 0x4, sizeof(nop2Bytes_4));
			mem::Nop(nopAddr2 + 0x28, sizeof(nop2Bytes_28));
			mem::Nop(nopAddr2 + 0x35, sizeof(nop2Bytes_35));
			mem::Nop(nopAddr2 + 0x43, sizeof(nop2Bytes_43));
		}
		/*
		Should make reticule more accurate but it messes up camera when near a wall
		*/
		//if (nopAddr3)
		//	mem::Nop((BYTE*)nopAddr3, sizeof(nop3Bytes));

		if (nopAddr3_2)
			mem::Nop(nopAddr3_2, sizeof(nop3_2Bytes));

		if (nopAddr3_3)
			mem::Nop(nopAddr3_3, sizeof(nop3_3Bytes));

		if (combatModeAddr)
			mem::Nop(combatModeAddr + 0x7, sizeof(combatModeBytes));

		if (instantFlickAddr)
			mem::Patch(instantFlickAddr + 0x7, &instantFlickByte_dis, sizeof(instantFlickByte_dis));
	}
}

void PatchTurnMode(ViewMode viewMode) {
	if (viewMode == FirstPerson) {
		if (!fpTurnMode) {
			BYTE turnModeBytes[1] = { 0xEB };
			if (turnModeAddr)
				mem::Patch(turnModeAddr, turnModeBytes, sizeof(turnModeBytes));
			fpTurnMode = true;
		}
	} else if (fpTurnMode) {
		BYTE turnModeBytes[1] = { 0x74 };

		if (turnModeAddr)
			mem::Patch(turnModeAddr, turnModeBytes, sizeof(turnModeBytes));
		fpTurnMode = false;
	}
}

void PatchBlackBars(bool disable) {
	if (!blackBarsAddr) {
		return;
	}

	if (disable) {
		BYTE blackBarsByte = 0xEB;
		mem::Patch(blackBarsAddr, &blackBarsByte, sizeof(blackBarsByte));
	} else {
		BYTE blackBarsByte = 0x75;
		mem::Patch(blackBarsAddr, &blackBarsByte, sizeof(blackBarsByte));
	}
}

void DisableGameCameraSwitching(bool disable) {
	BYTE nop4Bytes[6] = { 0x88, 0x9E, 0xA8, 0x00, 0x00, 0x00 };

	disableGameCameraSwitching = disable;

	if (!nopAddr4) {
		return;
	}

	if (disable) {
		mem::Nop(nopAddr4, sizeof(nop4Bytes));
	} else {
		mem::Patch(nopAddr4, nop4Bytes, sizeof(nop4Bytes));
	}
}

/*
void DisableAimingIK(bool disable) {
	BYTE enabledBytes[2] = { 0x74, 0x54 };
	BYTE enabledBytes_2[4] = { 0xEE, 0x13, 0x82, 0x00 };

	//BYTE disabledBytes[2] = { 0x90, 0x90 };
	BYTE disabledBytes_2[4] = { 0xFC, 0xFF, 0xFF, 0xFF };

	disableAimingIK = disable;

	if (disable) {
		if (aimingIKAddr) {
			mem::Nop(aimingIKAddr, 2);
			mem::Patch(aimingIKAddr + 10, disabledBytes_2, sizeof(disabledBytes_2));
		}
	} else {
		if (aimingIKAddr) {
			mem::Patch(aimingIKAddr, enabledBytes, sizeof(enabledBytes));
			mem::Patch(aimingIKAddr + 10, enabledBytes_2, sizeof(enabledBytes_2));
		}
	}
}
*/

void GetAddresses() {
	const char* camComboPattern = "F0 41 CD CC 4C 3D CD CC 4C 3D 66 66 66 3F 35 FA 8E 3C 00 00 00 00 00 00 00 00";
	cameraOffset = (uintptr_t)(mem::ScanIn(camComboPattern, (char*)moduleBase));

	const char* comboPattern = "D9 00 F3 0F 10 40 04 F3 0F 10 48 08 D9 1E F3 0F 11 46 04 F3 0F 11 4E 08 D9 40 0C D9 5E 0C 8B 43 34 85 C0 0F";
	nopAddr = (BYTE*)(mem::ScanIn(comboPattern, (char*)moduleBase));

	const char* comboPattern1 = "F3 0F 11 00 F3 0F 10 44 24 1C F3 0F 11 48 04 F3 0F 11 50 08 F3 0F 11 40 0C 5E 8B E5 5D C2 0C 00 CC";
	nopAddr1 = (BYTE*)(mem::ScanIn(comboPattern1, (char*)moduleBase));

	nopAddr2 = nopAddr1 - 0xCC;

	const char* comboPattern3 = "F3 0F 11 9E 5C 02 00 00 8B 87 64 15 00 00 85 C0";
	nopAddr3 = (BYTE*)(mem::ScanIn(comboPattern3, (char*)moduleBase));

	const char* comboPattern3_2 = "F3 0F 11 8E 5C 02 00 00 F3 0F 10";
	nopAddr3_2 = (BYTE*)(mem::ScanIn(comboPattern3_2, (char*)moduleBase));

	const char* comboPattern3_3 = "F3 0F 11 96 5C 02 00 00 E9";
	nopAddr3_3 = (BYTE*)(mem::ScanIn(comboPattern3_3, (char*)moduleBase));

	const char* comboPattern4 = "88 9E A8 00 00 00 5F 5E 5B C2 10 00";
	nopAddr4 = (BYTE*)(mem::ScanIn(comboPattern4, (char*)moduleBase));

	const char* combatModeComboPattern = "83 3d ?? ?? ?? ?? ?? 75 ?? 38 1d ?? ?? ?? ?? 75";
	combatModeAddr = (BYTE*)(mem::ScanIn(combatModeComboPattern, (char*)moduleBase));

	//const char* aimingComboPattern = "?? ?? 00 00 00 00 00 00 E8 ?? ?? ?? ?? FF 00 7F ?? 00";
	//aimingAddr = (BYTE*)(mem::ScanIn(aimingComboPattern, (char*)moduleBase));

	//char* turnModeComboPattern = "0f 94 ?? 8a c1 c2 ?? ?? cc cc cc cc cc cc cc cc cc cc 56 33 f6";
	//turnModeAddr = (BYTE*)(mem::ScanIn(turnModeComboPattern, (char*)moduleBase));

	const char* turnModeComboPattern = "74 ?? 83 e8 ?? 75 ?? 46";
	turnModeAddr = (BYTE*)(mem::ScanIn(turnModeComboPattern, (char*)moduleBase));

	const char* instantFlickComboPattern = "80 B9 ?? 00 00 00 00 74 0F 80 B9 ?? 00";
	instantFlickAddr = (BYTE*)(mem::ScanIn(instantFlickComboPattern, (char*)moduleBase));

	//const char* blackBarsComboPattern = "75 0C 80 3D ?? ?? ?? ?? 00";
	const char* blackBarsComboPattern = "75 56 F3 0F 10 05 ?? ?? ? ?? 0F";
	blackBarsAddr = (BYTE*)(mem::ScanIn(blackBarsComboPattern, (char*)moduleBase));

	const char* controllerFixComboPattern = "a2 ?? ?? ?? ?? 89 0d ?? ?? ?? ?? 89 15";
	controllerFixAddr = (BYTE*)(mem::ScanIn(controllerFixComboPattern, (char*)moduleBase));

	if (controllerFixAddr) {
		controllerFixAddr = *(BYTE**)(controllerFixAddr + 1);
	}
	//aimingIKAddr = (BYTE*)(moduleBase + 0x225908);
}

void SetGameplayCameraLocation(float x, float y, float z, float d) {
	gameplayCamera.x = x;
	gameplayCamera.y = y;
	gameplayCamera.z = z;
	//gameplayCamera.d = d;

	*cameraAddr = gameplayCamera;
	//*(float*)(cameraAddr + 10) = gameplayCamera.d;
}

void ShowPlayerHead(bool show, bool force = false) {
	if (!show && currentViewMode == ThirdPerson)
		return;

	if (!hideHead && !force)
		return;

	Ped plrPed = PLAYER::GET_PLAYER_PED(PLAYER::GET_PLAYER_ID());

	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_HEAD, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_HAIR, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_TEEF, show);
	PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_DECL, show); // ch 13 blood on face

	if (PED::GET_PED_DRAWABLE_VARIATION(plrPed, ePedComponent::PED_COMPONENT_TASK) == 5 ||
	    PED::GET_PED_DRAWABLE_VARIATION(plrPed, ePedComponent::PED_COMPONENT_TASK) == 6 || show)
		PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_TASK, show);

	if (PED ::GET_PED_DRAWABLE_VARIATION(plrPed, ePedComponent::PED_COMPONENT_SUSE) == 0 ||
	    PED::GET_PED_DRAWABLE_VARIATION(plrPed, ePedComponent::PED_COMPONENT_SUSE) == 11 || show)
		PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_SUSE, show);

	if (PED::GET_PED_DRAWABLE_VARIATION(plrPed, ePedComponent::PED_COMPONENT_SUS2) <= 1 || show)
		PLAYER::DISPLAY_PLAYER_COMPONENT(ePedComponent::PED_COMPONENT_SUS2, show);

	if (!show)
		PlayerProps.hat = PED::GET_PED_PROP_INDEX(plrPed, 0);
	if (PlayerProps.hat != -1 && !show) {
		PED::CLEAR_PED_PROP(plrPed, 0);
	}

	if (PlayerProps.hat != -1 && show) {
		PED::SET_PED_PROP_INDEX(plrPed, 0, PlayerProps.hat, 0, 0);
	}

	if (!show)
		PlayerProps.glasses = PED::GET_PED_PROP_INDEX(plrPed, 2);
	if (PlayerProps.glasses != -1 && !show) {
		PED::CLEAR_PED_PROP(plrPed, 2);
	}

	if (PlayerProps.glasses != -1 && show) {
		PED::SET_PED_PROP_INDEX(plrPed, 2, PlayerProps.glasses, 0, 0);
	}
}
bool IsInCutscene() {
	return CAM::IS_ANIMATED_CAMERA_PLAYING() || CUTSCENE::GET_CUTSCENE_TIME_MS() > 0;
}

bool FirstPersonAllowed(Player player) {
	Ped plrPed = PLAYER::GET_PLAYER_PED(player);
	int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();

	if (!CAM::IS_SCREEN_FADED_IN())
		return false;

	if (!firstPersonCover && (PED::IS_PED_IN_COVER(plrPed) || inLMSfromCover) && (currentLevel != 11 || ISEQ::ISEQ_GET_STATE(1321278157) != 2))
		return false;

	// this check is for some scripted events where you don't want to be in fp
	if (!IsInCutscene() && !PLAYER::IS_PLAYER_CONTROL_ON(player) && abs(CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING()) > 150)
		return false;

	if (currentLevel == 21 && ISEQ::ISEQ_GET_STATE(-1709834613) == 3) // ch 14, cp 11
		return false;

	// fire part first stairs = -211988314 ?
	if (currentLevel == 12 && ISEQ::ISEQ_GET_STATE(1252772788) == 3) // ch 6, plank
		return false;

	if (currentLevel == 11 && ISEQ::ISEQ_GET_STATE(-548128243) > 0 && ISEQ::ISEQ_GET_STATE(812276792) == 2 &&
	    !PLAYER::IS_PLAYER_CONTROL_ON(player)) // ch 6, lobby jump
		return false;

	if (currentLevel == 17 &&
	    (ISEQ::ISEQ_GET_STATE(-384669200) == 3 || (ISEQ::ISEQ_GET_STATE(123250951) > 0 && !PLAYER::IS_PLAYER_CONTROL_ON(player)))) // ch 10, bus
		return false;

	if (currentLevel == 5 && ISEQ::ISEQ_GET_STATE(94722883) == 3 && CAM::GET_RENDERING_CAM() != firstPersonCamera && CAM::GET_RENDERING_CAM() != -1) // ch 2
		return false;

	return !IsInCutscene() && !CAM::IS_BULLET_CAMERA_ACTIVE() && !CAM::IS_BULLET_CAMERA_SCENE_ACTIVE(CAM::GET_RENDERING_CAM()) &&
	    !PLAYER::IS_PLAYER_DOING_MELEE_GRAPPLE(player) && !PED::IS_PED_SWIMMING(plrPed) && !HUD::IS_SNIPER_SCOPE_VISIBLE() &&
	    !HUD::IS_PAUSE_MENU_ACTIVE() && !PLAYER::IS_PLAYER_CLIMBING(player) && PED::IS_PED_VISIBLE(plrPed) && !PLAYER::IS_PLAYER_DEAD(player) &&
	    (CAM::IS_CAM_RENDERING(firstPersonCamera) || !CAM::IS_CAM_SHAKING(CAM::GET_RENDERING_CAM()));
}

bool ShouldResetStreaming(Vector3 playerHeadPosition) {
	int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();
	//59 105
	if (currentLevel == 14 && (int)playerHeadPosition.x == 22 && (int)playerHeadPosition.y == -258) // ch 7, cp 6
		return true;

	return false;
}

void SetupViewMode(ViewMode viewMode, Vector3 cameraLocation, Vector3 cameraRotation, float cameraFOV, bool transition) {
	Vector3 gameplayCameraLocation = CAM::GET_GAMEPLAY_CAM_COORD();
	bool fpTransitionAllowed = !wasScoped && !wasInBulletCam && !wasInMelee;
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
		lerpedPos = cameraLocation;
		SetGameplayCameraLocation(lerpedPos.x, lerpedPos.y, lerpedPos.z, -1.04719758f);

		if (!CAM::DOES_CAM_EXIST(firstPersonCamera)) {
			firstPersonCamera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", cameraLocation.x, cameraLocation.y, cameraLocation.z,
			    cameraRotation.x, cameraRotation.y, cameraRotation.z, cameraFOV, 1);
		}

		if (transition && fpTransitionAllowed) {
			interpolatingCamera = true;
			interpolateStart = MISC::GET_GAME_TIMER();
		} else {
			ShowPlayerHead(false);
		}

		// fix for controller right stick not working after cutscenes
		if (controllerFixAddr) {
			*controllerFixAddr = 0;
		}
		CAM::SET_CAM_ACTIVE(firstPersonCamera, 1);
		CAM::RENDER_SCRIPT_CAMS(1, transition && fpTransitionAllowed, interpTime, 0); // camera to render, 1 = scripted camera, 0 = gameplay camera

		PatchBlackBars(true);
	} else {
		ShowPlayerHead(true);

		if (CAM::GET_RENDERING_CAM() == firstPersonCamera || CAM::GET_RENDERING_CAM() == -1)
			CAM::RENDER_SCRIPT_CAMS(0, tpTransitionAllowed, interpTime, 0);

		if (!disableBlackBars)
			PatchBlackBars(false);
	}
}

bool trainer_switch_pressed() {
	return IsKeyJustDown(VK_F2) || (isTrainerVisible && mainMenuActive && (IsKeyJustDown(VK_NUMPAD0) || IsKeyJustDown(VK_BACK)));
}

void process_options_menu(std::string& caption, std::vector<MenuItem>& lines, int* active_index, MenuNavigation& navigation) {
	if (navigation == NAV_Select) {
		switch (*active_index) {
		case 0:
			WritePrivateProfileStringA("SETTINGS", "FIRST_PERSON_COVER", firstPersonCover ? "1" : "0", ".\\FirstPerson.ini");
			break;
		case 1:
			WritePrivateProfileStringA(
			    "SETTINGS", "PREFER_TWO_HANDED_WEAPON_AFTER_CUTSCENE", preferTwoHandedWeaponAfterCutscene ? "1" : "0", ".\\FirstPerson.ini");
			break;
		case 2:
			ShowPlayerHead(!hideHead, 1);
			WritePrivateProfileStringA("ADVANCED", "HIDE_HEAD_FIRST_PERSON", hideHead ? "1" : "0", ".\\FirstPerson.ini");
			break;
		}
	} else {
		float delta = 1.0f;
		if (navigation == NAV_Scroll_L)
			delta *= -1;

		if (navigation == NAV_Scroll_L || navigation == NAV_Scroll_R) {
			if (*active_index == 3) {
				firstPersonFOV += delta;
				
				if (firstPersonFOV > MAX_FOV || firstPersonFOV < MIN_FOV) {
					firstPersonFOV = max(min(firstPersonFOV, MAX_FOV), MIN_FOV);
				} else {
					menu_beep(NAV_Scroll);
					CAM::SET_CAM_FOV(firstPersonCamera, firstPersonFOV);
					WritePrivateProfileStringA(
					    "SETTINGS", "FIRST_PERSON_FOV", std::to_string((int)firstPersonFOV).c_str(), ".\\FirstPerson.ini");
				}
			} else if (*active_index == 4) {
				firstPersonFOVReduction += delta;

				if (firstPersonFOVReduction > 50 || firstPersonFOVReduction < 0) {
					firstPersonFOVReduction = max(min(firstPersonFOVReduction, 50), 0);
				} else {
					menu_beep(NAV_Scroll);
					
					WritePrivateProfileStringA(
					    "SETTINGS", "FIRST_PERSON_AIMING_FOV_REDUCTION", std::to_string((int)firstPersonFOVReduction).c_str(), ".\\FirstPerson.ini");
				}
			}
			
		}
	}
}

void process_advanced_options_menu(std::string& caption, std::vector<MenuItem>& lines, int* active_index, MenuNavigation& navigation) {
	if (navigation == NAV_Select) {
		switch (*active_index) {
		case 3:
			offset.x = defaultOffset.x;
			offset.y = defaultOffset.y;
			offset.z = defaultOffset.z;

			WritePrivateProfileStringA("ADVANCED", "OFFSET_X", std::to_string(offset.x).c_str(), ".\\FirstPerson.ini");
			WritePrivateProfileStringA("ADVANCED", "OFFSET_Y", std::to_string(offset.y).c_str(), ".\\FirstPerson.ini");
			WritePrivateProfileStringA("ADVANCED", "OFFSET_Z", std::to_string(offset.z).c_str(), ".\\FirstPerson.ini");
			break;
#ifdef _DEBUG
		case 4:
			WritePrivateProfileStringA("ADVANCED", "DEBUG", debug ? "1" : "0", ".\\FirstPerson.ini");
			break;
#endif
		}
	} else {
		bool changed = false;

		float delta = 0.02;
		if (navigation == NAV_Scroll_L)
			delta *= -1;

		if (navigation == NAV_Scroll_L || navigation == NAV_Scroll_R) {
			switch (*active_index) {
			case 0:
				offset.x += delta;
				WritePrivateProfileStringA("ADVANCED", "OFFSET_X", std::to_string(offset.x).c_str(), ".\\FirstPerson.ini");
				break;
			case 1:
				offset.y += delta;
				WritePrivateProfileStringA("ADVANCED", "OFFSET_Y", std::to_string(offset.y).c_str(), ".\\FirstPerson.ini");
				break;
			case 2:
				offset.z += delta;
				WritePrivateProfileStringA("ADVANCED", "OFFSET_Z", std::to_string(offset.z).c_str(), ".\\FirstPerson.ini");
				break;
			}
			menu_beep(NAV_Scroll);
		}
	}
}

void process_misc_menu(std::string& caption, std::vector<MenuItem>& lines, int* active_index, MenuNavigation& navigation) {
	if (navigation == NAV_Select) {
		switch (*active_index) {
		case 0:
			WritePrivateProfileStringA("SETTINGS", "HIDE_PLAYER_DAMAGE", hidePlayerDamage ? "1" : "0", ".\\FirstPerson.ini");
			break;
		case 1:
			WritePrivateProfileStringA("SETTINGS", "DISABLE_BLACK_BARS", disableBlackBars ? "1" : "0", ".\\FirstPerson.ini");
			if (currentViewMode == ViewMode::ThirdPerson)
				PatchBlackBars(disableBlackBars);
			break;
		}
	}
}

int activeLineIndexMain = 0;

Menu menus[] = {
	{
		"GENERAL  OPTIONS",
	    {
			{ "First Person Cover (BUGGY)", 1, 0, &firstPersonCover },
			{ "Prefer Two Handed Weapons", 1, 0, &preferTwoHandedWeaponAfterCutscene },
			{ "Hide Head in First Person", 1, 0, &hideHead },
			{ "First Person FOV", 0, 1, 0, &firstPersonFOV },
			{ "Aiming FOV Reduction", 0, 1, 0, &firstPersonFOVReduction }
		}, &process_options_menu
	},
	{
	"ADVANCED  OPTIONS",
	{
		{ "OFFSET X", 0, 1, 0, &offset.x },
		{ "OFFSET Y", 0, 1, 0, &offset.y },
		{ "OFFSET Z", 0, 1, 0, &offset.z },
		{ "Reset to defaults" }
#ifdef _DEBUG
		,
		{ "ENABLE DEBUG", 1, 0, &debug }
#endif
	    }, &process_advanced_options_menu
	},
	{
		"MISC  OPTIONS",
		{
			{ "HIDE PLAYER DAMAGE", 1, 0, &hidePlayerDamage },
			{ "Disable Black Bars (Ultrawide)", 1, 0, &disableBlackBars }
		},
	    &process_misc_menu
	},
};

void process_main_menu() {
	const float lineWidth = 400.0;
	const float lineHeight = 20.0;
	const float lineSpacing = 58.0;
	const float spaceFromTitle = 68.0;
	const float lineLeft = 18.0;

	std::string caption = "First  Person  Mod";

	const int lineCount = sizeof(menus) / sizeof(*menus);

	bool bSelect, bBack, bUp, bDown, bLeft, bRight;
	get_button_state(&bSelect, &bBack, &bUp, &bDown, &bLeft, &bRight);

	MenuNavigation nav = NAV_None;

	if (bSelect)
		nav = NAV_Select;
	else if (bBack || bUp)
		nav = NAV_Scroll;
	else if (bLeft)
		nav = NAV_Scroll_L;
	else if (bRight)
		nav = NAV_Scroll_R;

	if (mainMenuActive) {
		draw_menu_line(caption, "", lineWidth, 15.0, 18.0, lineLeft, 5.0, false, true);
		for (int i = 0; i < lineCount; i++)
			draw_menu_line(
			    menus[i].caption, "", lineWidth, lineHeight, spaceFromTitle + i * lineSpacing, lineLeft, 9.0, i == activeLineIndexMain, false);

		// process buttons
		if (bSelect) {
			menu_beep(NAV_Select);
			mainMenuActive = false;
		} else if (bBack || trainer_switch_pressed()) {
			menu_beep(NAV_Back);
			//break;
		} else if (bUp) {
			menu_beep(NAV_Scroll);
			if (activeLineIndexMain == 0)
				activeLineIndexMain = lineCount;
			activeLineIndexMain--;
		} else if (bDown) {
			menu_beep(NAV_Scroll);
			activeLineIndexMain++;
			if (activeLineIndexMain == lineCount)
				activeLineIndexMain = 0;
		}
	} else {
		Menu& m = menus[activeLineIndexMain];

		if (bBack) {
			menu_beep(NAV_Back);
			mainMenuActive = true;
		} else if (bUp) {
			menu_beep(NAV_Scroll);
			if (m.active_index == 0)
				m.active_index = m.lines.size();
			(m.active_index)--;
		} else if (bDown) {
			menu_beep(NAV_Scroll);
			(m.active_index)++;
			if (m.active_index == m.lines.size())
				m.active_index = 0;
		}

		if (bSelect) {
			if (m.lines[m.active_index].togglable) {
				menu_beep(NAV_Toggle);
				if (m.lines[m.active_index].tState)
					*m.lines[m.active_index].tState = !(*m.lines[m.active_index].tState);
				//if (m.lines[m.active_index].pUpdated)
				m.lines[m.active_index].pUpdated = true;
			} else {
				menu_beep(NAV_Select);
			}
		}

		m.process_menu_function(m.caption, m.lines, &m.active_index, nav);
		draw_menu(m.caption, &m.lines[0], m.lines.size(), m.active_index);
	}
}

float GetRotationOffset() {
	int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();

	if (currentLevel == 7 && ISEQ::ISEQ_GET_STATE(1066718456) == 3)
		return -6;

	return 0;
}

float lerp(float& A, float& B, float t) {
	return A * t + B * (1.f - t);
}

int main() {
	char tempFloat[32];

	bool enabled = GetPrivateProfileIntA("SETTINGS", "ENABLED", 1, ".\\FirstPerson.ini") != 0;
	desiredViewMode = GetPrivateProfileIntA("SETTINGS", "FIRST_PERSON_DEFAULT_ENABLED", 1, ".\\FirstPerson.ini") != 0 ? FirstPerson : ThirdPerson;
	GetPrivateProfileStringA("SETTINGS", "FIRST_PERSON_FOV", "60.0", tempFloat, sizeof(tempFloat), ".\\FirstPerson.ini");
	firstPersonFOV = strtof(tempFloat, nullptr);
	GetPrivateProfileStringA("SETTINGS", "FIRST_PERSON_AIMING_FOV_REDUCTION", "0.0", tempFloat, sizeof(tempFloat), ".\\FirstPerson.ini");
	firstPersonFOVReduction = strtof(tempFloat, nullptr);
	firstPersonCover = GetPrivateProfileIntA("SETTINGS", "FIRST_PERSON_COVER", 0, ".\\FirstPerson.ini");
	int changeViewModeKey = GetPrivateProfileIntA("SETTINGS", "CHANGE_VIEW_MODE_KEY", 0x56, ".\\FirstPerson.ini");
	//hideHud = GetPrivateProfileIntA("SETTINGS", "HIDE_HUD", 0, ".\\FirstPerson.ini") != 0;
	preferTwoHandedWeaponAfterCutscene = GetPrivateProfileIntA("SETTINGS", "PREFER_TWO_HANDED_WEAPON_AFTER_CUTSCENE", 1, ".\\FirstPerson.ini") != 0;
	hidePlayerDamage = GetPrivateProfileIntA("SETTINGS", "HIDE_PLAYER_DAMAGE", 0, ".\\FirstPerson.ini") != 0;
	disableBlackBars = GetPrivateProfileIntA("SETTINGS", "DISABLE_BLACK_BARS", 1, ".\\FirstPerson.ini") != 0;

#ifdef _DEBUG
	debug = GetPrivateProfileIntA("ADVANCED", "DEBUG", 0, ".\\FirstPerson.ini") != 0;
#endif

	hideHead = GetPrivateProfileIntA("ADVANCED", "HIDE_HEAD_FIRST_PERSON", 1, ".\\FirstPerson.ini") != 0;

	Vector3 forwardOffset = Vector3::zero();

	GetPrivateProfileStringA("ADVANCED", "OFFSET_X", std::to_string(defaultOffset.x).c_str(), tempFloat, 32, ".\\FirstPerson.ini");
	offset.x = strtof(tempFloat, nullptr);

	GetPrivateProfileStringA("ADVANCED", "OFFSET_Y", std::to_string(defaultOffset.y).c_str(), tempFloat, 32, ".\\FirstPerson.ini");
	offset.y = strtof(tempFloat, nullptr);

	GetPrivateProfileStringA("ADVANCED", "OFFSET_Z", std::to_string(defaultOffset.z).c_str(), tempFloat, 32, ".\\FirstPerson.ini");
	offset.z = strtof(tempFloat, nullptr);

	bool wasCutscene = false;
	bool wasCover = false;

	float forward = 0.02;
	float headVisibleOffset = 0;
	int lastHeadVisibleCheck = 0;

	if (!enabled)
		return 0;

	PatchFirstPerson();
	GetAddresses();
	PatchBlackBars(disableBlackBars);

	while (true) {
		if (PLAYER::DOES_MAIN_PLAYER_EXIST()) {
			Player player = PLAYER::GET_PLAYER_ID();
			Ped plrPed = PLAYER::GET_PLAYER_PED(player);

			update_status_text();

			int currentLevel = MISC::GET_INDEX_OF_CURRENT_LEVEL();

			if (currentLevel == 115 || GRAPHICS::IS_TRANSITION_MOVIE_PLAYING()) { // frontend
				scriptWait(100);
				isTrainerVisible = false;
				continue;
			}

			if (trainer_switch_pressed()) {
				menu_beep(NAV_Back);
				isTrainerVisible = !isTrainerVisible;
			}

			if (isTrainerVisible)
				process_main_menu();

			if (hidePlayerDamage) {
				PED::RESET_PED_VISIBLE_DAMAGE(plrPed);
				WEAPON::CLEAR_PED_LAST_WEAPON_DAMAGE(plrPed);

				GRAPHICS::BLOOD_DECAL_ON_PED_BONE(plrPed, 0.00000000, 0.20000000, -999.00000000, 3229, 0);
				GRAPHICS::BLOOD_DECAL_ON_PED_BONE(plrPed, 0.00000000, 0.20000000, -999.00000000, 65488, 0);
				GRAPHICS::BLOOD_DECAL_ON_PED_BONE(plrPed, 0.00000000, 0.20000000, -999.00000000, 57704, 0);
			}

			if (HUD::IS_PAUSE_MENU_ACTIVE() && currentViewMode == FirstPerson && !interpolatingCamera) {
				HUD::SET_FRONTEND_ACTIVE(0);
				CAM::RENDER_SCRIPT_CAMS(0, 1, 3000, 0);
				scriptWait(0);
				HUD::SET_FRONTEND_ACTIVE(1);
				scriptWait(100);
			}

			if (IsKeyJustDown(VK_F9)) {
				MISC::RETURN_TO_TITLESCREEN(NULL);
			}
			cameraAddr = (CameraInfo*)(*(uintptr_t*)(cameraOffset + 0x1e) + 0x170);

			//Vector3 gameplayCameraLocation = CAM::GET_GAMEPLAY_CAM_COORD();
			Vector3 gameplayCameraRotation = CAM::GET_GAMEPLAY_CAM_ROT();

			float heading = PED::GET_PED_HEADING(plrPed);
			float camRelativeHeading = CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING();

			int wpn = WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0);
			int wpnType = WEAPON::GET_WEAPON_TYPE(wpn);
			int gunType = WEAPON::GET_WEAPON_GUNTYPE(wpnType);

			float lookUpScale = gameplayCameraRotation.x / 45.f;

			Vector3 extraOffset = Vector3::zero();
			Vector3 lookUpOff = Vector3(0, 0.06, 0);
			Vector3 lookDnOff = Vector3(0, -0.1, 0);
			Vector3 lookUpCOff = Vector3(0, 0, 0);

			if (gunType == GUNTYPE_RIFLE || gunType == GUNTYPE_ROCKET || gunType == GUNTYPE_SNIPER || gunType == GUNTYPE_MACHINEGUN) {
				//extraOffset = Vector3(-0.06, -0.24, 0);
			} else if (PED::IS_PED_DUAL_WIELDING(plrPed)) {
				extraOffset = Vector3(0.04, -0.04, -0.1);
				//lookUpOff = Vector3(-0.1, -0.28, 0);
			} else {
				//lookUpOff = Vector3(-0.1, -0.28, 0);
			}
			if (gunType == GUNTYPE_ROCKET) {
				lookUpOff = Vector3(0, -0.10, 0);
				extraOffset = Vector3(0, 0.2, 0.16);
			}

			if (!PED::IS_PED_DUCKING(plrPed)) {
				if (lookUpScale > 0)
					extraOffset = extraOffset + (lookUpOff * lookUpScale);
				else
					extraOffset = extraOffset + (lookDnOff * lookUpScale);
			}

			//extraOffset = Vector3::zero();

			Vector3 playerHeadPosition =
			    PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::HEAD, offset.x + extraOffset.x, offset.y + extraOffset.y, offset.z + extraOffset.z);
			Vector3 acc = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::HEAD, 0, 0, 0);
			Vector3 playerFootPosition = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::R_FOOT, 0, 0, 0);
			Vector3 playerNeckPosition = PED::GET_PED_BONE_COORDS(plrPed, (int)ePedBone::NECK, 0, 0, 0);

			float groundZ;
			MISC::GET_GROUND_Z_FOR_3D_COORD(playerHeadPosition.x, playerHeadPosition.y, playerHeadPosition.z, &groundZ);

			bool grounded = playerHeadPosition.z - groundZ < 1;
			bool rolling = grounded && !PLAYER::IS_PLAYER_SHOOTDODGING() && !PED::IS_PED_RAGDOLL(plrPed, 0) && !TASK::IS_PED_GETTING_UP(plrPed) &&
			    !PED::IS_PED_IN_COVER(plrPed);

#ifdef _DEBUG
			if (debug) {
				Vector3 vecV = PED::GET_PED_VELOCITY(plrPed);
				Vector3 n = vecV.normalize();
				//n = PED::GET_PED_DIRECTION(plrPed, 0);

				float f = MISC::GET_HEADING_FROM_VECTOR_2D(vecV.x, vecV.y);
				bool h = abs(f - heading) < 50;

				char array[128];
				//snprintf(array, sizeof(array), "%i %i %i %i Speed: %f", (int)camRelativeHeading, (int)heading, CAM::IS_CAM_RENDERING(FirstPersonCamera), CAM::GET_RENDERING_CAM(), PED::GET_PED_SPEED(plrPed));
				//snprintf(array, sizeof(array), "X: %f Y: %f Z: %f", playerHeadPosition.x, playerHeadPosition.y, playerHeadPosition.z);

				bool s = CAM::IS_SPHERE_VISIBLE(acc.x, acc.y, acc.z, 0.07, 1);

				(void)snprintf(array, sizeof(array), "X: %i Y: %i Z: %i, Addr %f, Far %f",
				    WEAPON::GET_WEAPON_FROM_SLOT(plrPed, 1) == WEAPON::GET_WEAPON_FROM_HAND(plrPed, 1, 0), s, rolling, camRelativeHeading,
				    forward);
				//snprintf(array, sizeof(array), "Lvl: %i Seq1: %i Seq2: %i Seq3: %i Seq4: %i Seq5: %i Seq6F: %i Seq7: %i FP: %i", currentLevel,
				//    ISEQ::ISEQ_GET_STATE(-850158086), ISEQ::ISEQ_GET_STATE(-1988558453), ISEQ::ISEQ_GET_STATE(320396699),
				//    ISEQ::ISEQ_GET_STATE(-236629402), ISEQ::ISEQ_GET_STATE(372227415), (int)playerHeadPosition.x,
				//    (bool)(PLAYER::IS_PLAYER_CONTROL_ON(player) && ISEQ::ISEQ_GET_STATE(1381054646) > 0), OBJECT::DOES_OBJECT_EXIST(camO));
				//2, 2, 2;    2,3,2;     0,3,0
				//snprintf(
				//    array, sizeof(array), "TimerA %i, TimerB: %i, SysTimer: %i", SYSTEM::TIMERA(), SYSTEM::TIMERB(), SYSTEM::TIMERSYSTEM());

				bool open =
				    CAM::CAMERA_ANIMATED_CURRENT_TIME() >= ((CAM::CAMERA_ANIMATED_LENGTH("LEVEL_S_FAV2_CP_12", "VIG_17_F2_B_CAM")) - 100);
				//snprintf(array, sizeof(array), "TimerA %i, TimerB: %i, State: %i",
				//   CAM::CAMERA_ANIMATED_LENGTH("LEVEL_S_FAV2_CP_12", "VIG_17_F2_B_CAM"), CAM::CAMERA_ANIMATED_CURRENT_TIME(),
				//  CUTSCENE::GET_CUTSCENE_TIME_MS());

				//if (CUTSCENE::GET_CUTSCENE_TIME_MS() > 0)
				//	MISC::SET_TIME_SCALE(0.01);
				//snprintf(array, sizeof(array), "Rend %i, Firstperson: %i, Scripted: %i, FP Cam %i", CAM::GET_RENDERING_CAM(),
				//    currentViewMode == FirstPerson, *(int*)(moduleBase + 0x1491174), firstPersonCamera);

				//snprintf(array, sizeof(array), "X: %f Y: %f Z: %f, State %i", offset.x, offset.y, offset.z, st);

				if (PED::IS_PED_PICKING_UP_PICKUP(plrPed) && (PED::IS_PED_IN_COVER(plrPed) || wasCover))
					PLAYER::SET_PLAYER_USING_COVER(player, true);
				//HUD::PRINT_STRING_WITH_LITERAL_STRING_NOW("STRING", array, 100, 1);

				if (IsKeyJustDown(VK_NUMPAD2))
					forward -= 0.01;
				if (IsKeyJustDown(VK_NUMPAD8))
					forward += 0.01;
				//PED::SET_HEADING_LIMIT_FOR_ATTACHED_PED(plrPed, 90, 90);

				//PLAYER::SET_FORCED_AIM_INTENTION_DIRECTION(n.x, 0, 0);
				//PLAYER::SET_FREEZE_HEADING_BLEND(1);
				//TASK::TASK_ACHIEVE_HEADING(plrPed, gameplayCameraRotation.z, 0, 10);

				Vector3 fpVel = Vector3::zero();
				float xmul = 0;

				if (PAD::IS_CONTROL_PRESSED(0, INPUT_MOVE_UP))
					xmul += 1;
				else if (PAD::IS_CONTROL_PRESSED(0, INPUT_MOVE_DOWN))
					xmul -= 1;

				fpVel.x = 2 * xmul;
				//PED::SET_HEADING_LIMIT_FOR_ATTACHED_PED(plrPed, 1, 1);
				//PED::SET_PED_HEADING(plrPed, gameplayCameraRotation.z);
				//PED::SET_PED_VELOCITY(plrPed, fpVel.x, fpVel.y, fpVel.z);

				//snprintf(array, sizeof(array), "X: %f Y: %i Z: %f", PED::GET_PED_SPEED(plrPed), h, heading);
				//snprintf(array, sizeof(array), "Type: %i Type2: %i AType: %i Wpn: %i", wpnType, gunType, wpnAmmoType, wpn);

				HUD::PRINT_STRING_WITH_LITERAL_STRING_NOW("STRING", array, 100, 1);
			}
#endif
			
			if (IsKeyJustDown(changeViewModeKey) || (PAD::IS_USING_CONTROLLER() && PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_SELECT))) {
				desiredViewMode = (ViewMode)!desiredViewMode;
				WritePrivateProfileStringA(
				    "SETTINGS", "FIRST_PERSON_DEFAULT_ENABLED", desiredViewMode == FirstPerson ? "1" : "0", ".\\FirstPerson.ini");
			}

			if (interpolatingCamera) {
				if (MISC::GET_GAME_TIMER() >
				    interpolateStart + interpTime * MISC::GET_TIME_SCALE()) { // CAM::IS_CAM_INTERPOLATING(FirstPersonCamera) is not working
					interpolatingCamera = false;

					ShowPlayerHead(false);
				}
			}

			if (FirstPersonAllowed(player)) {
				if (currentViewMode != desiredViewMode && !interpolatingCamera) {
					SetupViewMode(desiredViewMode, playerHeadPosition, gameplayCameraRotation, firstPersonFOV, 1);

					if (currentLevel == 13 && !disableGameCameraSwitching && !WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0))
						DisableGameCameraSwitching(1);

					//DisableAimingIK(true);
				}
			} else {
				if (currentViewMode == FirstPerson)
					SetupViewMode(ThirdPerson);
			}

			// get times
			if (PED::IS_PED_SHOOTING(plrPed))
				lastShotTime = MISC::GET_GAME_TIMER();

			//HUD::FORCE_RED_RETICULE(currentViewMode == FirstPerson && PLAYER::IS_PLAYER_TARGETTING_ANYTHING(player));

			// first person rendering
			if (currentViewMode == FirstPerson) {
				if (!CAM::IS_CAM_RENDERING(firstPersonCamera))
					SetupViewMode(FirstPerson, playerHeadPosition, gameplayCameraRotation, firstPersonFOV, interpolatingCamera);

				if (ShouldResetStreaming(playerHeadPosition))
					STREAMING::RESET_STREAMING_POINT_OF_INTEREST();

				// aiming fov
				float targetFOV = PAD::IS_CONTROL_PRESSED(0, INPUT_AIM) ? max(firstPersonFOV - firstPersonFOVReduction, MIN_FOV) : firstPersonFOV;
				lerpedFOV = lerp(lerpedFOV, targetFOV, exp(-10.0 * SYSTEM::TIMESTEPUNWARPED()));
				CAM::SET_CAM_FOV(firstPersonCamera, lerpedFOV);

				forwardOffset = Vector3::zero();
				float forwardScale = 0;

				if (!PED::IS_PED_IN_COVER(plrPed) ||
				    (PED::IS_PED_IN_COVER(plrPed) && lastHeadVisibleCheck + 100 * MISC::GET_TIME_SCALE() < MISC::GET_GAME_TIMER())) {
					if (CAM::IS_SPHERE_VISIBLE(acc.x, acc.y, acc.z, lookUpScale > 0.25 ? 0.02 : 0.07, 1)) {
						headVisibleOffset += 0.1;
					} else {
						if (PED::IS_PED_IN_COVER(plrPed))
							headVisibleOffset -= 0.005;
						else
							headVisibleOffset = 0;
					}
					lastHeadVisibleCheck = MISC::GET_GAME_TIMER();
				}

				if (lookUpScale > 0 && !PLAYER::IS_PLAYER_SHOOTDODGING()) {
					if (gunType == GUNTYPE_RIFLE || gunType == GUNTYPE_ROCKET || gunType == GUNTYPE_SNIPER ||
					    gunType == GUNTYPE_MACHINEGUN) {
						forwardScale = 0.3 * lookUpScale;
					} else if (PED::IS_PED_DUAL_WIELDING(plrPed)) {
						forwardScale = 0.35 * lookUpScale;
					} else {
						forwardScale = 0.35 * lookUpScale;
					}
				}

				forwardScale += forward;
				if (offset.y > -0.1) {
					forwardScale -= headVisibleOffset;
				}

				if (rolling) {
					//forwardOffset.x += 0.2f;
					//forwardOffset.z += 0.2f;
					forwardScale = -0.2;
				}

				double headingD = DegreesToRadians(gameplayCameraRotation.z);

				lerpedForwardScale = lerp(lerpedForwardScale, forwardScale, exp(-10.0 * SYSTEM::TIMESTEPUNWARPED())); //-25

				forwardOffset.x -= lerpedForwardScale * sin(-headingD);
				forwardOffset.y -= lerpedForwardScale * cos(-headingD);

				//	forwardOffset.z += 0.02 * sin(-DegreesToRadians(gameplayCameraRotation.x));

				if (lerpedPos.z == 0)
					lerpedPos = playerHeadPosition;

				if (PED::GET_PED_SPEED(plrPed) > 0 && PED::GET_PED_SPEED(plrPed) < 5 && !rolling)
					lerpedPos = Vector3::lerp(lerpedPos, playerHeadPosition, exp(-25.0 * SYSTEM::TIMESTEPUNWARPED())); // 0.89 old value
				else
					lerpedPos = playerHeadPosition;

				if (rolling && gameplayCameraRotation.x < -35) {
					//gameplayCameraRotation.x = 45;
					CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(-35, 1065353216, 0);
				}

				if (PLAYER::IS_PLAYER_SHOOTDODGING() && grounded && gameplayCameraRotation.x < -18) {
					//gameplayCameraRotation.x = 45;
					//CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(-18, 1065353216, 0);
				}

				CAM::SET_CAM_COORD(
				    firstPersonCamera, lerpedPos.x + forwardOffset.x, lerpedPos.y + forwardOffset.y, lerpedPos.z + forwardOffset.z);
				CAM::SET_CAM_ROT(firstPersonCamera, gameplayCameraRotation.x, gameplayCameraRotation.y,
				    gameplayCameraRotation.z + GetRotationOffset()); // up, left

				if (cameraAddr && CAM::IS_CAM_RENDERING(firstPersonCamera)) {
					SetGameplayCameraLocation(
					    lerpedPos.x + forwardOffset.x, lerpedPos.y + forwardOffset.y, lerpedPos.z + forwardOffset.z, -1.04719758f);
					float TempCamRelativeHeading = camRelativeHeading;
					float limit = 80;

					if ((PED::IS_PED_IN_COVER(plrPed) && MISC::GET_GAME_TIMER() > lastShotTime + 200) ||
					    !WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0)) {
						if (TempCamRelativeHeading < -limit)
							TempCamRelativeHeading = -limit;
						else if (TempCamRelativeHeading > limit)
							TempCamRelativeHeading = limit;
						if (!PLAYER::IS_PLAYER_LOCKED_IN_COVER(plrPed))
							CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(TempCamRelativeHeading, 1065353216, 0);
					}
				}

				PatchTurnMode(PED::IS_PED_VAULTING(plrPed) ||
					    WEAPON::GET_WEAPON_FROM_SLOT(plrPed, 2) == WEAPON::GET_WEAPON_FROM_HAND(plrPed, 0, 0) ||
					    PED::IS_PED_DUAL_WIELDING(plrPed) || PED::GET_PED_SPEED(plrPed) < 2
					? ThirdPerson
					: FirstPerson);
			} else {
				interpolateStart = -1;
				interpolatingCamera = false;

				if (disableGameCameraSwitching)
					DisableGameCameraSwitching(0);

				//if (disableAimingIK)
				//	DisableAimingIK(false);

				PatchTurnMode(ThirdPerson);
			}

			if (PLAYER::IS_LAST_MAN_STANDING_ACTIVE()) {
				if (wasCover)
					inLMSfromCover = true;
			} else {
				inLMSfromCover = false;
			}

			wasCover = PED::IS_PED_IN_COVER(plrPed);
			wasScoped = HUD::IS_SNIPER_SCOPE_VISIBLE();
			wasInBulletCam = CAM::IS_BULLET_CAMERA_ACTIVE();
			wasInMelee = PLAYER::IS_PLAYER_DOING_MELEE_GRAPPLE(player);

			if (!IsInCutscene() && wasCutscene) {
				// just exited cutscene

				wasCutscene = false;

				if (preferTwoHandedWeaponAfterCutscene && PLAYER::IS_PLAYER_CONTROL_ON(player) && MISC::GET_TIME_SCALE() > 0.9) {
					int wep = WEAPON::GET_WEAPON_FROM_HOLSTER(plrPed, 2);
					int wepType = WEAPON::GET_WEAPON_TYPE(wep);

					if (wep && (float)WEAPON::GET_WEAPON_AMMO_IN_CLIP(wep) / (float)WEAPON::GET_MAX_AMMO_IN_CLIP(plrPed, wepType) >= 0.25)
						WEAPON::SELECT_WEAPON_TO_HAND(plrPed, wep, 0, 0);
				}
			}

			if (IsInCutscene() && !wasCutscene) {
				// just entered cutscene

				wasCutscene = true;
			}

			if (hideHud) {
				HUD::DISPLAY_HUD(0);
			}
			//end if player exists
		}
		scriptWait(0);
	}

	return 0;
}

void ScriptMain() {
#ifdef CONSOLE_ENABLED
	AllocConsole();

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	HWND ConsoleHwnd = GetConsoleWindow();
	int ConsoleWidth = 250;
	int ConsoleHeight = 300;

	SetWindowPos(ConsoleHwnd, nullptr, desktop.right - ConsoleWidth, desktop.bottom - ConsoleHeight - 28, ConsoleWidth, ConsoleHeight,
	    SWP_NOZORDER); //SWP_NOSIZE
	HANDLE hOutConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hOutConsole, FOREGROUND_GREEN);

	SetConsoleCtrlHandler(nullptr, TRUE);

	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	printf("Started\n");
#endif
	main();
}
