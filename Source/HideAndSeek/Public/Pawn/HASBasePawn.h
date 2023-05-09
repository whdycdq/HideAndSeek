// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProtobufLibrary.h"
#include "GameFramework/Pawn.h"
#include "HASBasePawn.generated.h"

UCLASS()
class HIDEANDSEEK_API AHASBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHASBasePawn();
	virtual void SetPawnBehavior(const FBroadCast& InputBroadcast);
	UPROPERTY(BlueprintReadWrite)
	bool IsNetwork;
	UPROPERTY(BlueprintReadWrite)
	FBroadCast Broadcast;
	UPROPERTY(BlueprintReadWrite)
	FPlayerInfo Info;
	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void UpdateBroadcast();
	
	virtual void Jump();
};
