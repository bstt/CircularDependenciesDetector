// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesLib.h"
#include "AssetRegistryModule.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BlueprintEditor.h"
#include "CircularInvolvedAssetItem.h"
#include "../Config/CDD_EditorConfig.h"
#include "Editor.h"
#include "EdGraph/EdGraph.h"
#include "EngineUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "ImaginaryBlueprintData.h"
#include "Internationalization/Regex.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Notifications/SNotificationList.h"
#include <Config/CDD_ProjectConfig.h>
#include "Interfaces/IPluginManager.h"

void UCircularDependenciesLib::SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms)
{
	auto assetEditorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Asset, false);
	if (Asset->IsA<UBehaviorTree>())
	{
		FNotificationInfo notifInfo(NSLOCTEXT("CircularDependenciesDetector", "CircularDependenciesDetector", ""));
		FString message = FString("Search not available for Behaviour Tree");
		notifInfo.Text = FText::FromString(message);
		notifInfo.ExpireDuration = 3.0f;
		notifInfo.Image = FCoreStyle::Get().GetBrush(TEXT("MessageLog.Warning"));
		FSlateNotificationManager::Get().AddNotification(notifInfo);
		UE_LOG(LogTemp, Warning, TEXT("%s, contact me if you know how to implement it"), *message);
	}
	else //if (Asset->IsA<UBlueprint>())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Class name : %s"), *Asset->GetClass()->GetDisplayNameText().ToString());
		if (IBlueprintEditor* blueprintEditor = StaticCast<IBlueprintEditor*>(assetEditorInstance))
			blueprintEditor->SummonSearchUI(!bAllBlueprints, NewSearchTerms);
	}
}

UClass* UCircularDependenciesLib::GetClassFromAsset(UObject* Asset)
{
	if (!IsValid(Asset)) return nullptr;

	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		return blueprint->GeneratedClass;
	return nullptr;
}

TArray<FString> UCircularDependenciesLib::GetAllFunctions(UObject* Asset)
{
	TArray<FString> result;

	if (!IsValid(Asset)) return result;
	
	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		for (auto p : blueprint->FunctionGraphs)
			result.Add(p->GetName());

	return result;
}

bool UCircularDependenciesLib::IsInGameOrSlateThread()
{
	return IsInGameThread() || IsInSlateThread();
}

void UCircularDependenciesLib::ExecuteTask(const FVoidDelegate& toExecute, bool bInBackground, bool bWait)
{
	volatile bool isDone = false;
	
	// toExecute must be passed as copy because...
	AsyncTask(bInBackground ? ENamedThreads::AnyBackgroundThreadNormalTask : ENamedThreads::GameThread,
		[toExecute, &isDone] { toExecute.ExecuteIfBound(); isDone = true; });

	if (bWait)
		while (!isDone) {} // wait
}

bool UCircularDependenciesLib::RegexFind(const FString& pattern, const FString& input)
{
	return FRegexMatcher(FRegexPattern(pattern), input).FindNext();
}

void UCircularDependenciesLib::AddToDependencyStack(const TArray<FString>& assetPackageArray, const FName& CurrentAsset, UPARAM(ref) TMap<FName, FNameArray>& DependencyListMap,
	UPARAM(ref) TArray<FName>& DependencyStack, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet,
	UPARAM(ref) TArray<UCircularInvolvedAssetItem*>& circularInvolvedItemArray, UPARAM(ref) FBoolHolder& isStopping)
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetRegistry")) return;
	DependencyStack.Add(CurrentAsset);
	static FAssetRegistryModule& assetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FName> outDependencies;

	static const FRegexPattern externalPattern(TEXT("/__External[^/]*__/"));

	if (isStopping.value
		|| !assetPackageArray.FindByPredicate([&CurrentAsset](FString assetPackage) { return CurrentAsset.ToString().StartsWith(assetPackage); })
		|| FRegexMatcher(externalPattern, CurrentAsset.ToString()).FindNext())
	{
		DependencyStack.RemoveAt(DependencyStack.Num() - 1);
		return;
	}
	
	if (const FNameArray* pDependencyList = DependencyListMap.Find(CurrentAsset))
		outDependencies = pDependencyList->content;
	else
	{
		if (!getExcludedAssetList().Contains(CurrentAsset.ToString()))
			assetRegistryModule.GetRegistry().GetDependencies(CurrentAsset, outDependencies,
				UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::FDependencyQuery(UE::AssetRegistry::EDependencyQuery::Hard));
		DependencyListMap.Add(CurrentAsset, FNameArray(outDependencies));
	}
	for (const auto& childAsset : outDependencies)
	{
		if (!assetPackageArray.FindByPredicate([&childAsset](FString assetPackage) { return childAsset.ToString().StartsWith(assetPackage); })
			|| FRegexMatcher(externalPattern, childAsset.ToString()).FindNext())
			continue;
		FNamePair currentDependency = FNamePair(CurrentAsset, childAsset);
		if (childAsset.ToString() == CurrentAsset.ToString() || BrokenDependecySet.Contains(currentDependency))
			continue;
		int childAssetIndex = DependencyStack.Find(childAsset);
		if (childAssetIndex == INDEX_NONE)
			AddToDependencyStack(assetPackageArray, childAsset, DependencyListMap, DependencyStack, BrokenDependecySet,
				circularInvolvedItemArray, isStopping);
		else
		{
			FNamePair oppositeDependency = FNamePair(childAsset, CurrentAsset);
			BrokenDependecySet.Add(currentDependency);
			BrokenDependecySet.Add(oppositeDependency);
			TArray<FName> involvedDependencyStack;
			for (int i = childAssetIndex; i < DependencyStack.Num(); i++)
				involvedDependencyStack.Add(DependencyStack[i]);
			UCircularInvolvedAssetItem* circularInvolvedItem = NewObject<UCircularInvolvedAssetItem>();
			circularInvolvedItem->AssetName = involvedDependencyStack[0];
			circularInvolvedItem->DependencyStack = involvedDependencyStack;
			circularInvolvedItemArray.Add(circularInvolvedItem);
		}
	}
	DependencyStack.RemoveAt(DependencyStack.Num() - 1);
}

int UCircularDependenciesLib::getMaxDetectionCount()
{
	return UCDD_EditorConfig::Get()->maxDetectionCount;
}

float UCircularDependenciesLib::getAutomaticRefreshDelay()
{
	return UCDD_EditorConfig::Get()->automaticRefreshDelay;
}

TArray<FPlugin> UCircularDependenciesLib::GetEnabledPlugins()
{
	TArray<FPlugin> result;
	for (const auto& plugin : IPluginManager::Get().GetEnabledPlugins())
		result.Add(FPlugin(plugin->GetName(), plugin->GetBaseDir()));
	return result;
}

const TArray<FString>& UCircularDependenciesLib::getPluginList()
{
	return UCDD_ProjectConfig::Get()->pluginList;
}

void UCircularDependenciesLib::addPlugin(const FString& plugin)
{
	UCDD_ProjectConfig::Get()->pluginList.AddUnique(plugin);
	UCDD_ProjectConfig::Get()->SaveConfig();
}

void UCircularDependenciesLib::removePlugin(const FString& plugin)
{
	UCDD_ProjectConfig::Get()->pluginList.Remove(plugin);
	UCDD_ProjectConfig::Get()->SaveConfig();
}

const TArray<FString>& UCircularDependenciesLib::getExcludedAssetList()
{
	return UCDD_ProjectConfig::Get()->excludedAssetList;
}

bool UCircularDependenciesLib::addExcludedAsset(const FString& excludedAsset)
{
	bool result = !UCDD_ProjectConfig::Get()->excludedAssetList.Contains(excludedAsset); // true if not already contained
	if (result)
	{
		UCDD_ProjectConfig::Get()->excludedAssetList.Add(excludedAsset);
		UCDD_ProjectConfig::Get()->SaveConfig();
	}
	return result;
}

bool UCircularDependenciesLib::removeExcludedAsset(const FString& excludedAsset)
{
	bool result = UCDD_ProjectConfig::Get()->excludedAssetList.Remove(excludedAsset) > 0;
	UCDD_ProjectConfig::Get()->SaveConfig();
	return result;
}
