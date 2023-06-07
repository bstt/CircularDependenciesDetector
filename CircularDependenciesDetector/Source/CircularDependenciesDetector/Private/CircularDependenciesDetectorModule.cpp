// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesDetectorModule.h"
#include "Config/CDD_Settings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilityWidget.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FCircularDependenciesDetectorModule"

void FCircularDependenciesDetectorModule::StartupModule()
{
	CDD_Settings::RegisterSettings();
	if (CDD_Settings::ShowOnStartup())
		SpawnCDD();
}

void FCircularDependenciesDetectorModule::ShutdownModule()
{
	CDD_Settings::UnregisterSettings();
}

void FCircularDependenciesDetectorModule::SpawnCDD()
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	assetRegistryModule.Get().OnFilesLoaded().AddLambda([]() {
		if (!IsValid(GEditor)) return;
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

		if (!IsValid(EditorUtilitySubsystem)) return;
		const FSoftObjectPath ewbpPath("/CircularDependenciesDetector/CircularDep/CircularDependencies_EWBP.CircularDependencies_EWBP");
		UObject* Blueprint = ewbpPath.TryLoad();

		if (!IsValid(Blueprint)) return;
		UEditorUtilityWidgetBlueprint* EditorWidget = Cast<UEditorUtilityWidgetBlueprint>(Blueprint);

		if (!IsValid(EditorWidget)) return;
		FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

		auto spawnCddTab = [EditorUtilitySubsystem, EditorWidget]()
		{
			UEditorUtilityWidget* UtilityWidget = EditorUtilitySubsystem->FindUtilityWidgetFromBlueprint(EditorWidget);
			if (UtilityWidget)
				UtilityWidget->Run();
			else
				EditorUtilitySubsystem->SpawnAndRegisterTab(EditorWidget);
		};

		if (LevelEditorModule.GetLevelEditorTabManager().IsValid())
			spawnCddTab();
		else
			LevelEditorModule.OnLevelEditorCreated().AddLambda([spawnCddTab](TSharedPtr<ILevelEditor> levelEditor) { spawnCddTab(); });
	});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCircularDependenciesDetectorModule, CircularDependenciesDetector)