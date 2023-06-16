// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Plugin.generated.h"

USTRUCT(BlueprintType)
struct FPlugin
{
	GENERATED_USTRUCT_BODY();

	FPlugin() {}

	FPlugin(const FString& name_, const FString& baseDir_) : name(name_), baseDir(baseDir_) {}

	UPROPERTY(BlueprintReadWrite, Category = "Plugin")
		FString name;

	UPROPERTY(BlueprintReadWrite, Category = "Plugin")
		FString baseDir;
};
