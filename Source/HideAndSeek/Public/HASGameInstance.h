// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HASTcpSocketConnection.h"
#include "Engine/GameInstance.h"
#include "HASGameInstance.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAsyncLoadFinished);

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHASGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	float GetLoadProgress();
protected:
	
	bool bIsLoaded;
	UPROPERTY(BlueprintReadWrite)
	AHASTcpSocketConnection* Socket;
	UFUNCTION(BlueprintCallable)
	void LoadAsset(const FOnAsyncLoadFinished& OnAsyncLoadFinished);
	FString PackagePath;
	void OnAssetsLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result);
private:
	FStringAssetReference AssetRef;
};
