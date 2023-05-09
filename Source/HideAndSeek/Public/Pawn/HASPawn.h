// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/PawnInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pawn/HASBasePawn.h"
#include "HASPawn.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API AHASPawn : public APawn,public IPawnInterface
{
	GENERATED_BODY()
public:
	AHASPawn();
	UPROPERTY(BlueprintReadWrite)
	bool IsNetwork;
	UPROPERTY(BlueprintReadWrite)
	FBroadCast Broadcast;
	UPROPERTY(BlueprintReadWrite)
	FPlayerInfo Info;
	UPROPERTY(BlueprintReadWrite)
	FAxisStruct Axis;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UCharacterMovementComponent* Movement;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UBoxComponent* BoxComponent;
	virtual void SetPawnBehavior(const FBroadCast& InputBroadcast) override;
	void Jump();
	virtual void UpdateBroadcast() override;
	virtual void SetIsNetwork(bool bNetwork)override;
	virtual bool GetIsNetwork()override;
	virtual void SetBroadcast(const FBroadCast& bc)override;
	virtual FBroadCast& GetBroadcast()override;
	virtual void SetPlayerInfo(const FPlayerInfo& PlayerInfo)override;
	virtual FPlayerInfo& GetPlayerInfo()override;
	virtual void SetAxis(FAxisStruct Axis) override;
	virtual FAxisStruct& GetAxis() override;
};
