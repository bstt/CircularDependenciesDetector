// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UEditorUtilitySubsystem;
class UEditorUtilityWidgetBlueprint;

class FCircularDependenciesDetectorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void registerSpawnCDD();
};
