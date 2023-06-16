// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class CDD_Settings
{
	CDD_Settings() = delete;

public:
	static void RegisterSettings();
	static void UnregisterSettings();

	static bool bShowOnStartup();

private:
	static void resetConfigToDefaultValues(UObject* SettingsObject);
};