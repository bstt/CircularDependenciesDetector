// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoolHolder.generated.h"

USTRUCT(BlueprintType)
struct FBoolHolder
{
	GENERATED_USTRUCT_BODY();

	FBoolHolder() {}

	FBoolHolder(bool _value) : value(_value) {}

	UPROPERTY(BlueprintReadWrite, Category = "Bool Holder")
		bool value;
};
