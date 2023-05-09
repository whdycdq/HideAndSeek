// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HASWidgetBase.h"
#include "ProtobufLibrary.h"
#include "HASDelegateBus.h"
#include "HASChatWidge.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHASChatWidge : public UHASWidgetBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FOnInputAction InputCallback;
	UHASDelegateBus* Bus;
	UHASChatWidge();
	UHASChatWidge(FString ChildPath);
	UPROPERTY(BlueprintReadOnly)
	FString ChildPath;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerID;
	UFUNCTION(BlueprintImplementableEvent)
	void OnPressTab();
	UFUNCTION(BlueprintImplementableEvent)
	void AddPlayer(FPlayerInfo& PlayerInfo);
	UFUNCTION(BlueprintCallable)
	bool SendChat(FText Text,EChatType Type,TArray<int32>& uid);
	virtual void ShowUI() override;
	virtual void HideUI() override;
	virtual void Refresh() override;
	
};
