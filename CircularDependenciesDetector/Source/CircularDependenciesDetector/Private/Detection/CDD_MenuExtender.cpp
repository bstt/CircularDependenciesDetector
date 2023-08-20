// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CDD_MenuExtender.h"
#include "Detection/CircularDependenciesLib.h"
#include "ContentBrowserModule.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilityWidget.h"

CDD_MenuExtender& CDD_MenuExtender::Get()
{
	static CDD_MenuExtender instance;
	return instance;
}

void CDD_MenuExtender::Init()
{
	const FSoftObjectPath ewbpPath("/CircularDependenciesDetector/CircularDep/CircularDependencies_EWBP.CircularDependencies_EWBP");
	UObject* Blueprint = ewbpPath.TryLoad();

	this->editorWidget = IsValid(Blueprint) ? Cast<UEditorUtilityWidgetBlueprint>(Blueprint) : nullptr;
	this->registrationName = FName(*(Blueprint->GetPathName() + TEXT("_ActiveTab")));

	this->editorUtilitySubsystem = IsValid(GEditor) ? GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() : nullptr;
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	this->levelEditorModule = &LevelEditorModule;
}

void CDD_MenuExtender::SpawnCddTab()
{
	if (!this->editorUtilitySubsystem || !this->editorWidget) return;

	UEditorUtilityWidget* UtilityWidget = this->editorUtilitySubsystem->FindUtilityWidgetFromBlueprint(this->editorWidget);
	if (UtilityWidget)
		FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")).GetLevelEditorTabManager()->TryInvokeTab(this->registrationName);
	else
		this->editorUtilitySubsystem->SpawnAndRegisterTab(this->editorWidget);
}

FLevelEditorModule& CDD_MenuExtender::GetLevelEditorModule()
{
	return *this->levelEditorModule;
}

void CDD_MenuExtender::RegisterContextualMenu()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	auto& menuExtenders2 = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	menuExtenders2.Add(FContentBrowserMenuExtender_SelectedPaths::CreateLambda([this](const TArray<FString>& SelectedPath)
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

		MenuExtender->AddMenuExtension("PathContextBulkOperations", EExtensionHook::After, TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateLambda([this, SelectedPath](FMenuBuilder& menuBuilder) // copy of SelectedAssets required
		{
			menuBuilder.BeginSection("Circular Dependencies Detector", FText::FromString("Circular Dependencies Detector"));
			{
				menuBuilder.AddMenuEntry(
					FText::FromString("Detect For Selected"),
					FText::FromString("Start detection from selected assets"),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([&SelectedPath, this]()
				{
					this->SpawnCddTab();
					UCircularDependenciesLib::parseDelegate.ExecuteIfBound(TArray<FAssetData>(), SelectedPath);
				})));
			}
			menuBuilder.EndSection();
			menuBuilder.AddMenuSeparator();
		}));

		return MenuExtender.ToSharedRef();
	}));

	auto& menuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	menuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateLambda([this](const TArray<FAssetData>& SelectedAssets)
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

		MenuExtender->AddMenuExtension("AssetContextReferences", EExtensionHook::After, TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateLambda([this, SelectedAssets](FMenuBuilder& menuBuilder) // copy of SelectedAssets required
		{
			menuBuilder.BeginSection("Circular Dependencies Detector", FText::FromString("Circular Dependencies Detector"));
			{
				menuBuilder.AddMenuEntry(
					FText::FromString("Detect For Selected"),
					FText::FromString("Start detection from selected assets"),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([&SelectedAssets, this]()
				{
					this->SpawnCddTab();
					UCircularDependenciesLib::parseDelegate.ExecuteIfBound(SelectedAssets, TArray<FString>());
				})));
			}
			menuBuilder.EndSection();
			menuBuilder.AddMenuSeparator();
		}));
		return MenuExtender.ToSharedRef();
	}));
}