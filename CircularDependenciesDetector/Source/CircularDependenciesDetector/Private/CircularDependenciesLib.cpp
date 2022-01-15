// Copyright 2020 bstt, Inc. All Rights Reserved.

#include "CircularDependenciesLib.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "BlueprintEditor.h"
#include "Editor.h"

void UCircularDependenciesLib::SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms)
{
	IAssetEditorInstance* editorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Asset, false);
	FBlueprintEditor* blueprintEditor = StaticCast<FBlueprintEditor*>(editorInstance);
	if (!blueprintEditor)
		return;
	blueprintEditor->SummonSearchUI(!bAllBlueprints, NewSearchTerms);
}