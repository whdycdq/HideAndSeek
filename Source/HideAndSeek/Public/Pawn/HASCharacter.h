// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HASDelegateBus.h"
#include "Interface/PawnInterface.h"
#include "Pawn/HASBasePawn.h"
#include "HASCharacter.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API AHASCharacter :public ACharacter,public IPawnInterface
{
	
	GENERATED_BODY()

public:
	
	AHASCharacter();
	UPROPERTY(BlueprintReadWrite)
	bool IsNetwork;
	UPROPERTY(BlueprintReadWrite)
	FBroadCast Broadcast;
	UPROPERTY(BlueprintReadWrite)
	FPlayerInfo Info;
	UFUNCTION(BlueprintCallable)
	virtual void SetPawnBehavior(const FBroadCast& InputBroadcast) override;
	UFUNCTION(BlueprintCallable)
	virtual void UpdateBroadcast() override;
	UFUNCTION(BlueprintCallable)
	virtual void SetIsNetwork(bool bNetwork) override;
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsNetwork() override;
	UFUNCTION(BlueprintCallable)
	virtual void SetBroadcast(const FBroadCast& bc) override;
	UFUNCTION(BlueprintCallable)
	virtual FBroadCast& GetBroadcast() override;
	UFUNCTION(BlueprintCallable)
	virtual void SetPlayerInfo(const FPlayerInfo& PlayerInfo) override;
	UFUNCTION(BlueprintCallable)
	virtual FPlayerInfo& GetPlayerInfo() override;
	UFUNCTION(BlueprintCallable)
	virtual void SetAxis(const FAxisStruct Axis) override;
	UFUNCTION(BlueprintCallable)
	virtual FAxisStruct& GetAxis() override;
	UFUNCTION(BlueprintCallable)
	float GetSpeed(const FVector& Start,const FVector& Target);
	UPROPERTY(BlueprintReadWrite)
	float Speed;
	UPROPERTY(BlueprintReadWrite)
	FAxisStruct Axis;
	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation;
	UPROPERTY(BlueprintReadWrite)
	FVector TargetLoction;
	UPROPERTY(BlueprintReadWrite)
	bool IsJump;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;
};

