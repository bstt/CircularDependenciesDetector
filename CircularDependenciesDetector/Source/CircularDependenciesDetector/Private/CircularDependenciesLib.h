// Copyright 2020 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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

};
