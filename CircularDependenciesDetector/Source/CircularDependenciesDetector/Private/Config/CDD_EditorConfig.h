// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CDD_EditorConfig.generated.h"

/**
 *
 */
UCLASS(config = Editor)
class UCDD_EditorConfig : public UObject
{
	GENERATED_BODY()

public:

	FORCEINLINE static UCDD_EditorConfig* Get() { return GetMutableDefault<UCDD_EditorConfig>(); }

	// number of circular dependencies detected from which detection is interrupted
	UPROPERTY(EditAnywhere, config, Category = "Circular Dependencies Detector")
		uint32 maxDetectionCount = 100;

	// delay in seconds before an automatic refresh, put a negative value to disable the automatic refresh
	UPROPERTY(EditAnywhere, config, Category = "Circular Dependencies Detector")
		float automaticRefreshDelay = 1.f;
};