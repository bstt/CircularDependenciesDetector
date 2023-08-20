// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "LevelEditor.h"

class UEditorUtilitySubsystem;
class UEditorUtilityWidgetBlueprint;

/**
*
*/
class CDD_MenuExtender
{
public:
	static CDD_MenuExtender& Get();

	void Init();

	void SpawnCddTab();
	void RegisterContextualMenu();
	
	FLevelEditorModule& GetLevelEditorModule();

private:
	FName registrationName;
	UEditorUtilitySubsystem* editorUtilitySubsystem = nullptr;
	UEditorUtilityWidgetBlueprint* editorWidget = nullptr;
	FLevelEditorModule* levelEditorModule = nullptr;
};
