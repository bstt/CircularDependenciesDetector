// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CDD_ProjectConfig.generated.h"

/**
 *
 */
UCLASS(config = Project)
class UCDD_ProjectConfig : public UObject
{
	GENERATED_BODY()

public:

	FORCEINLINE static UCDD_ProjectConfig* Get() { return GetMutableDefault<UCDD_ProjectConfig>(); }

	// list of plugins to analyze
	UPROPERTY(EditAnywhere, config, meta = (DisplayName = "Plugins"), Category = "Circular Dependencies Detector")
		TArray<FString> pluginList;

	// list of excluded assets
	UPROPERTY(EditAnywhere, config, meta = (DisplayName = "Excluded assets"), Category = "Circular Dependencies Detector")
		TArray<FString> excludedAssetList;
};