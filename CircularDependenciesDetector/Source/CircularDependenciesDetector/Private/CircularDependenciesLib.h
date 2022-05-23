// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Async/Async.h"
#include "BoolHolder.h"
#include "Components/ListView.h"
#include "NameArray.h"
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

	DECLARE_DYNAMIC_DELEGATE(FVoidDelegate);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta = (Keywords = "find editor"))
		static void SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Circular Dependencies Lib")
		static UClass* GetClassFromAsset(UObject* Asset);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta = (Keywords = "method class"))
		static TArray<FString> GetAllFunctions(UObject* Asset);

	// Some actions like accessing Slate must be done in a GameThread or SlateThread
	// Use this function to check this
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Circular Dependencies Lib")
		static bool IsInGameOrSlateThread();

	// Some actions like spawining object cannot be done in background
	// Careful, bWait need to be enable in order to wait task execution whether in background or not 
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static void ExecuteTask(const FVoidDelegate& toExecute, bool bInBackground, bool bWait);

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static bool RegexFind(const FString& pattern, const FString& input);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static void AddToDependencyStack(FName CurrentAsset, UPARAM(ref) TMap<FName, FNameArray>& DependencyListMap,
			UPARAM(ref) TArray<FName>& DependencyStack, const TSet<FString>& ExcludedAssetSet, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet,
			UPARAM(ref) TArray<UCircularInvolvedAssetItem*>& circularInvolvedItemArray, UPARAM(ref) FBoolHolder& isStopping);
};
