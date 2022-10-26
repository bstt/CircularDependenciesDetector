// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CDD_Settings.h"
#include "CDD_EditorConfig.h"
#include "Copy_ApplicationRestartRequiredNotification.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Modules/ModuleManager.h"

void CDD_Settings::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
#define LOCTEXT_NAMESPACE "FCircularDependenciesDetectorModule"
		ISettingsSectionPtr EditorSettingsSection = SettingsModule->RegisterSettings("Editor", "Plugins", "Circular Dependencies Detector",
			LOCTEXT("RuntimeGeneralSettingsName", "Circular Dependencies Detector"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Editor configuration for Circular Dependencies Detector module"),
			UCDD_EditorConfig::Get()
		);
#undef LOCTEXT_NAMESPACE
		static FCopy_ApplicationRestartRequiredNotification ApplicationRestartRequiredNotification;
		EditorSettingsSection->OnModified().BindLambda([]()
		{
			UCDD_EditorConfig::Get()->SaveConfig();
			return true;
		});
		EditorSettingsSection->OnResetDefaults().BindLambda([EditorSettingsSection]()
		{
			resetConfigToDefaultValues(UCDD_EditorConfig::Get());
			ApplicationRestartRequiredNotification.OnRestartRequired();
			return true;
		});
	}
}

void CDD_Settings::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Circular Dependencies Detector");
}

bool CDD_Settings::ShowOnStartup()
{
	return UCDD_EditorConfig::Get()->bShowOnStartup;
}

void CDD_Settings::resetConfigToDefaultValues(UObject* SettingsObject)
{
	FString ConfigName = SettingsObject->GetClass()->GetConfigName();

	GConfig->EmptySection(*SettingsObject->GetClass()->GetPathName(), ConfigName);
	GConfig->Flush(false);

	FConfigCacheIni::LoadGlobalIniFile(ConfigName, *FPaths::GetBaseFilename(ConfigName), nullptr, true);

	SettingsObject->ReloadConfig(nullptr, nullptr, UE4::LCPF_PropagateToInstances | UE4::LCPF_PropagateToChildDefaultObjects);
}