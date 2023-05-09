// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "JsonSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UJsonSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UJsonSubsystem();
	TMap<FString,FString> WidgetMap;
	UFUNCTION(BlueprintCallable)
	void Reload();
	UFUNCTION(BlueprintCallable)
	bool ReloadWidgetJson();
	UFUNCTION(BlueprintCallable)
	bool GetWidgetPath(FString Name,FString& Path);
private:
	FString WidgetJsonPath = FPaths::ProjectContentDir() +TEXT("Json/widget.json");;
};
