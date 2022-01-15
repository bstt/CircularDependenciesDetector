// Copyright 2020 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

};
