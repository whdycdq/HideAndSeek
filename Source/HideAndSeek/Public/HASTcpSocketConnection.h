// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProtobufLibrary.h"
#include "TcpSocketConnection.h"
#include "HASDelegateBus.h"
#include "HASTcpSocketConnection.generated.h"
/**
 * 
 */

UCLASS()
class HIDEANDSEEK_API AHASTcpSocketConnection : public ATcpSocketConnection
{
	GENERATED_BODY()
public:
	AHASTcpSocketConnection();
	//三个父类需要绑定的调度器
	FTcpSocketDisconnectDelegate DisconnectedDelegate;
	FTcpSocketConnectDelegate ConnectedDelegate;
	FTcpSocketReceivedMessageDelegate MessageReceivedDelegate;
	//消息总线的指针
	UHASDelegateBus* Bus;
	//连接ID
	UPROPERTY(BlueprintReadWrite)
	int32 ConnectionID;
	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> MarshaledData;
	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> Message;
	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> SendMessage;
	
public:
	//事件调度器TcpSocketConnectDelegate对应的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnConnected(int32 ConnID);
	//事件调度器FTcpSocketDisconnectDelegate对应的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnDisconnected(int32 ConnID);
	//事件调度器TcpSocketReceivedMessageDelegate对应的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnReceivedMessage(int32 ConnID,UPARAM(ref) TArray<uint8>&Output);
	//事件调度器MessageTransmissionDelegate对应的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnMessageTransmission (EMessageType Type, const FBroadCast& OutBroadCast,TSubclassOf<UObject> SendClass);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTick(const FBroadCast& BroadCast);
	//发送消息的绑定函数
	UFUNCTION(BlueprintImplementableEvent)
	void OnSendChat(const FChat& Chat);
	//打包消息，把Protobuf序列化后的数据打包一个8字节的头
	UFUNCTION(BlueprintCallable)
	void Datapack(EMessageType Type,const TArray<uint8>&Input,TArray<uint8>&Output);
	//连接到服务器
	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& ipAddress, int32 port,int32& Connid);
	//发送广播消息给服务器
	UFUNCTION(BlueprintCallable)
	bool SendBroadcastToServer(EMessageType Type,const FBroadCast& BroadCast);
	//发送聊天消给服务器
	UFUNCTION(BlueprintCallable)
	bool SendChatToServer (const FChat& Chat);
	//发送消息给服务器，需要protobuf序列化后的数据
	UFUNCTION(BlueprintCallable)
	bool SendMessageToServer(EMessageType Type,const TArray<uint8>& Data);
	//处理服务器发来的消息
	UFUNCTION(BlueprintCallable)
	bool ProcessMessage(UPARAM(ref) TArray<uint8>&Input);
	//处理广播消息
	UFUNCTION(BlueprintCallable)
	bool ProcessBroadCastMessage(EMessageType Type,UPARAM(ref)TArray<uint8>&Input);
	//处理聊天消息
	UFUNCTION(BlueprintCallable)
	bool ProcessChatMessage(UPARAM(ref) TArray<uint8>&Input);
	
};
