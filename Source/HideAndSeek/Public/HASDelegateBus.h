// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Protobuf/ProtobufLibrary.h"
#include "UObject/NoExportTypes.h"
#include "HASDelegateBus.generated.h"


/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FChatDelegate,const FChat&,Chat);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSendChatDelegate,const FChat&,Chat);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLoginResponseReceived,const FBroadCast&,BroadCast);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FBroadCastDelegate,EMessageType,Type,const FBroadCast&,BroadCast);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTick,const FBroadCast&,BroadCast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMessageTransmissionDelegate,EMessageType,Type, const FBroadCast&,BroadCast,TSubclassOf<UObject>,SendClass);
UCLASS()
class HIDEANDSEEK_API UHASDelegateBus : public UObject
{
	GENERATED_BODY()
public:
	static UHASDelegateBus* GetInstance();
	FOnTick OnTick;
	FChatDelegate ChatDelegate;
	FSendChatDelegate SendChatDelegate;
	FBroadCastDelegate BroadCastDelegate;
	FOnLoginResponseReceived LoginResponseDelegate;
	UPROPERTY(BlueprintAssignable)
	FMessageTransmissionDelegate MessageTransmissionDelegate;
private:
	static UHASDelegateBus* HASDelegateBusPtr;
	UHASDelegateBus();
};
