// Fill out your copyright notice in the Description page of Project Settings.


#include "HASTcpSocketConnection.h"

#include "Chaos/ChaosPerfTest.h"
#include "Kismet/GameplayStatics.h"


AHASTcpSocketConnection::AHASTcpSocketConnection()
{
	Bus = UHASDelegateBus::GetInstance(); //获取消息总线
	//绑定函数到父类的调度器上
	DisconnectedDelegate.BindDynamic(this,&AHASTcpSocketConnection::OnDisconnected); 
	ConnectedDelegate.BindDynamic(this,&AHASTcpSocketConnection::OnConnected);
	MessageReceivedDelegate.BindDynamic(this,&AHASTcpSocketConnection::OnReceivedMessage);
	if (Bus)
	{
		Bus->SendChatDelegate.BindDynamic(this,&AHASTcpSocketConnection::OnSendChat);
		Bus->OnTick.BindDynamic(this,&AHASTcpSocketConnection::OnTick);
	}
}

	


void AHASTcpSocketConnection::Datapack(EMessageType Type,const TArray<uint8>&Input,TArray<uint8>&Output)
{
	//根据传进来的消息类型和消息长度拼接头
	TArray<uint8> Head =this->Concat_BytesBytes(this->Conv_IntToBytes(int32(Type)),this->Conv_IntToBytes(Input.Num()));
	//头拼接消息
	Output = this->Concat_BytesBytes(Head,Input);
}

bool AHASTcpSocketConnection::ConnectToServer(const FString& ipAddress, int32 port,int32& Connid)
{
	//连接服务器！！！！这个其实主要给蓝图用的
	Connect(ipAddress,port,DisconnectedDelegate,ConnectedDelegate,MessageReceivedDelegate,Connid);
	return true;
}

bool AHASTcpSocketConnection::SendBroadcastToServer(EMessageType Type, const FBroadCast& BroadCast)
{
	TArray<uint8> Output;
	if(UProtobufLibrary::MarshalBroadCast(BroadCast,Output)) //序列化Broadcast数据
	{
		if(SendMessageToServer(Type,Output)) //发送数据到server
		{
			return true;
		}
		return false;
	}
	return false;
}

bool AHASTcpSocketConnection::SendChatToServer(const FChat& Chat)
{
	TArray<uint8> Output;
	if (UProtobufLibrary::MarshalChat(Chat,Output))
	{
		if (SendMessageToServer(EMessageType::Chat,Output))
		{
			return true;
		}
		return false;
	}
	return false;
}

bool AHASTcpSocketConnection::SendMessageToServer(EMessageType Type, const TArray<uint8>& Data)
{
	TArray<uint8> SendBytes;
	Datapack(Type,Data,SendBytes); //打包头
	if(this->SendData(ConnectionID,SendBytes)) //发送数据
	{
		return true;
	}
	return false;
}

/*
 * 处理消息的函数，把服务器发来的消息解析头，然后把消息传送给具体的函数执行下一步解析
 */
bool AHASTcpSocketConnection::ProcessMessage(TArray<uint8>&Input)
{
	while (Input.Num() >= 8) //8字节的头，小于8字节别进
		{
		TArray<uint8> Output; //临时变量，用于解析后的数据
		TArray<uint8> MessageIDArray; //消息ID数组
		//先读取四字节，将消息类型解析出来
		Message_ReadBytes(4, Input, MessageIDArray);
		int32 MessageID = Message_ReadInt(MessageIDArray);
		//消息长度数组
		TArray<uint8> MessageLengthArray;
		//再读四字节，然后将长度解析出来
		Message_ReadBytes(4, Input, MessageLengthArray);
		int32 MessageLength = Message_ReadInt(MessageLengthArray);
		//根据长度，读取剩下的字节
		Message_ReadBytes(MessageLength, Input, Output);
		//根据消息ID的不同分配不同的处理方法
		switch (MessageID)
		{
		case 1:
			ProcessBroadCastMessage(EMessageType::PreLogin, Output);
			break;
		case 2:
			ProcessBroadCastMessage(EMessageType::SyncPlayer, Output);
			break;
		case 200:
			ProcessBroadCastMessage(EMessageType::BroadCast, Output);
			break;
		case 201:
			ProcessBroadCastMessage(EMessageType::KickPlayer, Output);
			break;
		case 202:
			ProcessChatMessage(Output);
			break;
		default:
			break;
		}
		}
	return Input.Num() == 0;
}

bool AHASTcpSocketConnection::ProcessBroadCastMessage(EMessageType Type, TArray<uint8>&Input)
{
	FBroadCast BroadCast;
	//处理BroadCast信息
	if(UProtobufLibrary::UnMarshalBroadCastByMessageType(Type,Input,BroadCast))
	{
		if (Bus)
		{
			//处理完的消息通过总线传给绑定了这个调度器的类
			Bus->BroadCastDelegate.Execute(Type,BroadCast);
		}
		return true;
	}
	else
	{
		if (Bus)
		{
			Bus->BroadCastDelegate.Execute(Type,BroadCast);
		}
		return false;
	}
}

bool AHASTcpSocketConnection::ProcessChatMessage(TArray<uint8>& Input)
{
	FChat Chat;
	if (UProtobufLibrary::UnMarshalChat(Input,Chat))
	{
		if (Bus)
		{
			Bus->ChatDelegate.Execute(Chat);
		}
		return true;
	}
	else
	{
		if (Bus)
		{
			Bus->ChatDelegate.Execute(Chat);
			return false;
		}
	}
	return false;

}

 


