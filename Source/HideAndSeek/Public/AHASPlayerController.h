// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProtobufLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Pawn/BlueprintReader/CharacterJsonSubsystem.h"
#include "AHASPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API AAHASPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAHASPlayerController();
	UPROPERTY(BlueprintReadWrite)
	FPlayerInfo Info;
	UPROPERTY(BlueprintReadWrite)
	FBroadCast BroadCast;
	UPROPERTY(BlueprintReadWrite)
	APawn* SpawnedPawn;
	UPROPERTY(BlueprintReadWrite)
	UCharacterJsonSubsystem* Sub;
	UPROPERTY(BlueprintReadWrite)
	bool IsNetwork;
	UFUNCTION(BlueprintCallable)
	void SetPlayerInfo(const FPlayerInfo& PlayerInfo);
	UFUNCTION(BlueprintCallable)
	bool Broadcast(const FBroadCast& BroadcastInput);
	UFUNCTION(BlueprintCallable)
	bool SpawnCharacter(ECharacterType Type, const FTransform& SpawnTrans,const FPlayerInfo& PlayerInfo,bool network);
	UFUNCTION(BlueprintCallable)
	bool SpawnPawn(EPawnType Type,const FTransform& SpawnTrans,bool network);
	UFUNCTION(BlueprintCallable)
	virtual void Tick(float DeltaSeconds) override;
};
