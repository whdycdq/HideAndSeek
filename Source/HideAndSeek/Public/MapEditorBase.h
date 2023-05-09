// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/HASActorBase.h"
#include "GameFramework/GameModeBase.h"
#include "MapEditorBase.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API AMapEditorBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SerializeMapInfo(const TArray<AHASActorBase*>& Actor);
	TSharedPtr<FJsonObject> ToAABBJsonValue( AHASActorBase* Actor);
	UPROPERTY(BlueprintReadWrite)
	FString Path = FPaths::ProjectContentDir() +TEXT("Json/map.json");
	TSharedPtr<FJsonObject> ToJsonValue( AHASActorBase* Actor);
	
};
