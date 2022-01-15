// Copyright 2020 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CircularInvolvedAssetItem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UCircularInvolvedAssetItem : public UObject
{
	GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, Category = "Circular Involved Asset Item")
        FName AssetName;

    UPROPERTY(BlueprintReadWrite, Category = "Circular Involved Asset Item")
        TArray<FName> DependencyStack;

};
