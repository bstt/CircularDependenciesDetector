// Copyright 2020 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/ListView.h"
#include "NamePair.h"
#include "CircularDependenciesLib.generated.h"

/**
 * 
 */
UCLASS()
class UCircularDependenciesLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta=(Keywords = "find editor"))
		static void SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Circular Dependencies Lib")
		static UClass* GetClassFromAsset(UObject* Asset);
	
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta = (Keywords = "method class"))
		static TArray<FString> GetAllFunctions(UObject* Asset);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies EWBP Lib")
		static void AddToDependencyStack(FName CurrentAsset, UPARAM(ref) TArray<FName>& DependencyStack,
			const TSet<FString>& ExcludedAssetSet, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet, UListView* InvolvedAssetListView);

};
