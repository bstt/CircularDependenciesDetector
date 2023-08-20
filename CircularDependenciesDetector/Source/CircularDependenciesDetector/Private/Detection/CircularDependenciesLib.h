// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Async/Async.h"
#include "BoolHolder.h"
#include "Components/ListView.h"
#include "NameArray.h"
#include "NamePair.h"
#include "Plugin.h"
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

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FParseDelegate, const TArray<FAssetData>&, toParseFileList, const TArray<FString>&, toParseFolderList);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta = (Keywords = "find editor"))
		static void SearchInBlueprint(UObject* Asset, bool bAllBlueprints, FString NewSearchTerms);

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static UClass* GetClassFromAsset(UObject* Asset);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib", meta = (Keywords = "method class"))
		static TArray<FString> GetAllFunctions(UObject* Asset);

	// Some actions like accessing Slate must be done in a GameThread or SlateThread
	// Use this function to check this
	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static bool IsInGameOrSlateThread();

	// Some actions like spawning object cannot be done in background
	// Careful, bWait need to be enable in order to wait task execution whether in background or not 
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static void ExecuteTask(const FVoidDelegate& toExecute, bool bInBackground, bool bWait);

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static bool RegexFind(const FString& pattern, const FString& input);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static void AddToDependencyStack(const TArray<FString>& assetPackageArray, const FName& CurrentAsset, UPARAM(ref) TMap<FName, FNameArray>& DependencyListMap,
			UPARAM(ref) TArray<FName>& DependencyStack, UPARAM(ref) TSet<FNamePair>& BrokenDependecySet,
			UPARAM(ref) TArray<UCircularInvolvedAssetItem*>& circularInvolvedItemArray, UPARAM(ref) FBoolHolder& isStopping);

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static int getMaxDetectionCount();

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib")
		static float getAutomaticRefreshDelay();

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static TArray<FPlugin> GetEnabledPlugins();

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib/Project setting")
		static const TArray<FString>& getPluginList();

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib/Project setting")
		static void addPlugin(const FString& plugin);

	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib/Project setting")
		static void removePlugin(const FString& plugin);

	UFUNCTION(BlueprintPure, Category = "Circular Dependencies Lib/Project setting")
		static const TArray<FString>& getExcludedAssetList();

	// returns true if asset has been added
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib/Project setting")
		static bool addExcludedAsset(const FString& excludedAsset);

	// returns true if asset has been removed
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib/Project setting")
		static bool removeExcludedAsset(const FString& excludedAsset);

	// function to execute one time, in order to bind a blueprint callback in c++
	UFUNCTION(BlueprintCallable, Category = "Circular Dependencies Lib")
		static void BindParseDelegate(const FParseDelegate& toBind);

	static inline FParseDelegate parseDelegate;
};
