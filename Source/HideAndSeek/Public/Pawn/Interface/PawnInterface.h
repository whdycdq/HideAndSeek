// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProtobufLibrary.h"
#include "PawnInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HIDEANDSEEK_API IPawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetPawnBehavior(const FBroadCast& InputBroadcast) = 0;
	virtual void UpdateBroadcast() = 0;
	virtual void SetIsNetwork(bool bNetwork) = 0;
	virtual bool GetIsNetwork() = 0;
	virtual void SetBroadcast(const FBroadCast& bc) = 0;
	virtual FBroadCast& GetBroadcast() = 0;
	virtual void SetPlayerInfo(const FPlayerInfo& PlayerInfo)=0;
	virtual FPlayerInfo& GetPlayerInfo() = 0;
	virtual void SetAxis(const FAxisStruct axis) = 0;
	virtual FAxisStruct& GetAxis() = 0;
};
