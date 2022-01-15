// Copyright 2022 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesLib.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "AssetRegistryModule.h"
#include "BlueprintEditor.h"
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

UClass* UCircularDependenciesLib::GetClassFromAsset(UObject* Asset)
{
	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		return blueprint->GeneratedClass;
	return nullptr;
}

TArray<FString> UCircularDependenciesLib::GetAllFunctions(UObject* Asset)
{
	TArray<FString> result;
	
	if (UBlueprint* blueprint = Cast<UBlueprint>(Asset))
		for (auto p : blueprint->FunctionGraphs)
			result.Add(p->GetName());

	return result;
}

void UCircularDependenciesLib::ExecuteTask(FVoidDelegate toExecute, bool bInBackground, bool bWait)
{
	volatile bool isDone = false;
	
	AsyncTask(bInBackground ? ENamedThreads::AnyBackgroundThreadNormalTask : ENamedThreads::GameThread,
		[toExecute, &isDone] { toExecute.ExecuteIfBound(); isDone = true; });

	if (bWait)
		while (!isDone) {} // wait
}

void UCircularDependenciesLib::AddToDependencyStack(FName CurrentAsset, UPARAM(ref) TMap<FName, FNameArray>& DependencyListMap,
	UPARAM(ref) TArray<FName>& DependencyStack, const TSet<FString>& ExcludedAssetSet, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet,
	UPARAM(ref) TArray<UCircularInvolvedAssetItem*>& circularInvolvedItemArray, UPARAM(ref) FBoolHolder& isStopping, FVoidDelegate toExecuteOnStep)
{
	DependencyStack.Add(CurrentAsset);
	static FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FName> outDependencies;

	if (isStopping.value || !CurrentAsset.ToString().StartsWith("/Game"))
	{
		DependencyStack.RemoveAt(DependencyStack.Num() - 1);
		return;
	}
	
	if (const FNameArray* pDependencyList = DependencyListMap.Find(CurrentAsset))
		outDependencies = pDependencyList->content;
	else
	{
		if (!ExcludedAssetSet.Contains(CurrentAsset.ToString()))
			assetRegistryModule.GetRegistry().GetDependencies(CurrentAsset, outDependencies,
				UE::AssetRegistry::EDependencyCategory::Package, UE::AssetRegistry::FDependencyQuery(UE::AssetRegistry::EDependencyQuery::Hard));
		DependencyListMap.Add(CurrentAsset, FNameArray(outDependencies));
		toExecuteOnStep.Execute();
	}
	for (auto childAsset : outDependencies)
	{
		if (!childAsset.ToString().StartsWith("/Game"))
			continue;
		FNamePair currentDependency = FNamePair(CurrentAsset, childAsset);
		if (childAsset.ToString() == CurrentAsset.ToString() || BrokenDependecySet.Contains(currentDependency))
			continue;
		int childAssetIndex = DependencyStack.Find(childAsset);
		if (childAssetIndex == INDEX_NONE)
			AddToDependencyStack(childAsset, DependencyListMap, DependencyStack, ExcludedAssetSet, BrokenDependecySet,
				circularInvolvedItemArray, isStopping, toExecuteOnStep);
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
