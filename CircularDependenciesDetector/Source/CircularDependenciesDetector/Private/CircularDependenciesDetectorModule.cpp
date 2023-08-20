// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesDetectorModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Detection/CircularDependenciesLib.h"
#include "Detection/CDD_MenuExtender.h"
#include "Config/CDD_Settings.h"

#define LOCTEXT_NAMESPACE "FCircularDependenciesDetectorModule"

void FCircularDependenciesDetectorModule::StartupModule()
{
	CDD_Settings::RegisterSettings();
	CDD_MenuExtender::Get().Init();
	CDD_MenuExtender::Get().RegisterContextualMenu();
	if (CDD_Settings::bShowOnStartup())
		registerSpawnCDD();
}

void FCircularDependenciesDetectorModule::ShutdownModule()
{
	CDD_Settings::UnregisterSettings();
}

void FCircularDependenciesDetectorModule::registerSpawnCDD()
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	assetRegistryModule.Get().OnFilesLoaded().AddLambda([this]() {
		auto& levelEditorModule = CDD_MenuExtender::Get().GetLevelEditorModule();
		if (levelEditorModule.GetLevelEditorTabManager().IsValid())
			CDD_MenuExtender::Get().SpawnCddTab();
		else
			levelEditorModule.OnLevelEditorCreated().AddLambda([](TSharedPtr<ILevelEditor> levelEditor) { CDD_MenuExtender::Get().SpawnCddTab(); });
	});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCircularDependenciesDetectorModule, CircularDependenciesDetector)