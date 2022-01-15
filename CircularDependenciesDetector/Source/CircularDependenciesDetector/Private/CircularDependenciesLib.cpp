// Copyright 2020 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesLib.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Editor.h"
#include "FindInBlueprints.h"
#include "ImaginaryBlueprintData.h"
#include "FindInBlueprintManager.h"
#include "EdGraph/EdGraph.h"
#include "EngineUtils.h"

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