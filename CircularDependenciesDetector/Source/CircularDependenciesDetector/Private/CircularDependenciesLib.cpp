// Copyright 2020 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesLib.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "AssetRegistryModule.h"
#include "CircularInvolvedAssetItem.h"
#include "Editor.h"
#include "EdGraph/EdGraph.h"
#include "EngineUtils.h"
#include "FindInBlueprintManager.h"
#include "FindInBlueprints.h"
#include "ImaginaryBlueprintData.h"

void UCircularDependenciesLib::SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms)
{
	if (FBlueprintEditor* blueprintEditor = StaticCast<FBlueprintEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Asset, false)))
		blueprintEditor->SummonSearchUI(!bAllBlueprints, NewSearchTerms);
}

TArray<FString> UCircularDependenciesLib::GetAllFunctions(UObject* Asset)
{
	TArray<FString> result;
	
	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		for (auto p : blueprint->FunctionGraphs)
			result.Add(p->GetName());

	return result;
}

UClass* UCircularDependenciesLib::GetClassFromAsset(UObject* Asset)
{
	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		return blueprint->GeneratedClass;
	return nullptr;
}

void UCircularDependenciesLib::AddToDependencyStack(FName CurrentAsset, UPARAM(ref) TArray<FName>& DependencyStack,
	const TSet<FString>& ExcludedAssetSet, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet, UListView* InvolvedAssetListView)
{
	if (ExcludedAssetSet.Contains(CurrentAsset.ToString()))
		return;
	DependencyStack.Add(CurrentAsset);
	static FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FName> outDependencies;
	assetRegistryModule.GetRegistry().GetDependencies(CurrentAsset, outDependencies, EAssetRegistryDependencyType::Hard);
	for (auto childAsset : outDependencies)
	{
		FNamePair currentDependency = FNamePair(CurrentAsset, childAsset);
		if (childAsset.ToString() == CurrentAsset.ToString() || BrokenDependecySet.Contains(currentDependency))
			continue;
		int childAssetIndex = DependencyStack.Find(childAsset);
		if (childAssetIndex == INDEX_NONE)
			AddToDependencyStack(childAsset, DependencyStack, ExcludedAssetSet, BrokenDependecySet, InvolvedAssetListView);
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
			InvolvedAssetListView->AddItem(circularInvolvedItem);
		}
	}
	DependencyStack.RemoveAt(DependencyStack.Num() - 1);
}
