// Fill out your copyright notice in the Description page of Project Settings.


#include "HASGameInstance.h"

#include "Engine/AssetManager.h"

void UHASGameInstance::LoadAsset(const FOnAsyncLoadFinished& OnAsyncLoadFinished)
{
	AssetRef.SetPath(TEXT("/Game/BigStarStation/Maps/Map_BigStarStation"));
	PackagePath = AssetRef.ToString();
	bIsLoaded = false;
 
	UE_LOG(LogTemp, Warning, TEXT("String: %s"), *PackagePath);
	LoadPackageAsync(
		PackagePath,
		FLoadPackageAsyncDelegate::CreateLambda([=](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result) 
			{
				if (Result == EAsyncLoadingResult::Failed)
				{
					UE_LOG(LogTemp, Warning, TEXT("Load Failed"));
				}
				else if (Result == EAsyncLoadingResult::Succeeded)
				{
					bIsLoaded = true;
					UE_LOG(LogTemp, Warning, TEXT("Load Succeeded"));
					OnAsyncLoadFinished.ExecuteIfBound();
				}
			}), 0, PKG_ContainsMap);


}

void UHASGameInstance::OnAssetsLoaded(const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
{


}

float UHASGameInstance::GetLoadProgress()
{
	float FloatPercentage = GetAsyncLoadPercentage(*PackagePath);
	if (!bIsLoaded)
	{
		FString ResultStr = FString::Printf(TEXT("Percentage: %f"), FloatPercentage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, ResultStr);
		UE_LOG(LogTemp, Warning, TEXT("Percentage: %f"), FloatPercentage);
	}
	else {
		FloatPercentage = 100;
	}
	return FloatPercentage;	
}