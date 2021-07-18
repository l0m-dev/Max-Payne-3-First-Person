#pragma once

#include "script.h"

enum ViewMode {
	FirstPerson,
	ThirdPerson
};

void SetupViewMode(ViewMode ViewMode, Vector3 CameraLocation = Vector3::zero(), Vector3 CameraRotation = Vector3::zero(), float CameraFOV = 60, bool transition = false);
void PatchGameplayCameraPosition(ViewMode ViewMode);
