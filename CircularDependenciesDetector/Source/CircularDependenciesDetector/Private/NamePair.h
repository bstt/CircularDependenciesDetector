// Copyright 2020 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NamePair.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNamePair
{
    GENERATED_USTRUCT_BODY();

    FNamePair() {}
    FNamePair(FName _Name1, FName _Name2) : Name1(_Name1), Name2(_Name2) {}

    UPROPERTY(BlueprintReadWrite, Category = "Name Pair")
        FName Name1;

    UPROPERTY(BlueprintReadWrite, Category = "Name Pair")
        FName Name2;

    FORCEINLINE bool operator==(const FNamePair& other) const
    {
        return Name1.ToString() == other.Name1.ToString() && Name2.ToString() == other.Name2.ToString();
    }

    friend FORCEINLINE uint32 GetTypeHash(const FNamePair& namePair)
    {
        return HashCombine(GetTypeHash(namePair.Name1.ToString()), GetTypeHash(namePair.Name2.ToString()));
    }

};
