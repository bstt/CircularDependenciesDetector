// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NameArray.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNameArray
{
    GENERATED_USTRUCT_BODY();

    FNameArray() {}

    FNameArray(const TArray<FName>& _content) : content(_content) {}

    UPROPERTY(BlueprintReadWrite, Category = "Name Array")
        TArray<FName> content;
};
