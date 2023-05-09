// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Protobuf/ProtobufLibrary.h"
#include "AHASPlayerController.h"
#include "HASTcpSocketConnection.h"
#include "HASDelegateBus.h"
#include "WidgetControllerComponent.h"
#include "HideAndSeekGameMode.generated.h"

UCLASS(minimalapi)
class AHideAndSeekGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//构造函数
	AHideAndSeekGameMode();
	//Socket连接Actor
	UPROPERTY(BlueprintReadWrite)
	AHASTcpSocketConnection* SocketConnection;
	//本地PlayerController
	UPROPERTY(BlueprintReadWrite)
	AAHASPlayerController* LocalPC;
	//GamemodeClass（废弃）
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AHideAndSeekGameMode>GamemodeClass;
	//PlayerController的Map
	UPROPERTY(BlueprintReadWrite)
	TMap<int32,AAHASPlayerController*> PCMap;
	UPROPERTY(BlueprintReadOnly)
	FTransform SocketSpawnTrans;
	//事件调度器MessageTransmission的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnMessageTransmission (EMessageType Type, const FBroadCast& BroadCast,TSubclassOf<UObject> SendClass);
	//事件调度器ReceivedBroadCast的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnReceivedBroadCast(EMessageType Type,const FBroadCast& BroadCast);
	//事件调度器ReceivedChat的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnReceivedChat(const FChat& Chat);
	UFUNCTION(BlueprintImplementableEvent)
	void OnLoginResponseReceived(const FBroadCast& BroadCast);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTCPConnected(int32 ConnID);
	//预登录函数
	UFUNCTION(BlueprintCallable)
	bool PreLogin(const FBroadCast& BroadCast);
	//同步玩家的函数
	UFUNCTION(BlueprintCallable)
	bool SyncPlayer(const FBroadCast& BroadCast);
	//广播函数
	UFUNCTION(BlueprintCallable)
	bool Broadcast(const FBroadCast& BroadCast);
	//踢出玩家的函数
	UFUNCTION(BlueprintCallable)
	bool KickPlayer(const FBroadCast& BroadCast);
	//tick
	UFUNCTION(BlueprintCallable)
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable)
	UJsonSubsystem* GetJsonSubsystem();
	UFUNCTION(BlueprintCallable)
	void testfunc(const int& hello,const int& world);
protected:
	virtual void BeginPlay() override;
};

