// Copyright Epic Games, Inc. All Rights Reserved.

#include "HideAndSeekGameMode.h"
#include "Pawn/Interface/PawnInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AHideAndSeekGameMode::AHideAndSeekGameMode()
{
	//Gamemode构造函数，这里获取了世界，然后生成了Socket以便网络通讯使用，然后通过消息总线绑定OnReceivedChat，OnReceivedBroadCast两个多播委托
	UWorld* const World = GetWorld(); //获取世界
	UHASDelegateBus* Bus = UHASDelegateBus::GetInstance();//获取消息总线
	if (World&&Bus)
	{
		ConstructorHelpers::FClassFinder<AHASTcpSocketConnection> BlueprintClassFinder(TEXT("/Game/BP_HASTcpSocketConnection")); //获取到socket的class
		if (BlueprintClassFinder.Succeeded())
		{
			//生成socket类，并保存起来
			SocketConnection= World->SpawnActor<AHASTcpSocketConnection>(BlueprintClassFinder.Class,FVector(0.f,0.f,0.f),FRotator(0.f,0.f,0.f));
		}
		//绑定多播委托
		Bus->ChatDelegate.BindDynamic(this,&AHideAndSeekGameMode::OnReceivedChat);
		Bus->BroadCastDelegate.BindDynamic(this,&AHideAndSeekGameMode::OnReceivedBroadCast);
		Bus->LoginResponseDelegate.BindDynamic(this,&AHideAndSeekGameMode::OnLoginResponseReceived);
		if (SocketConnection)
		{
			SocketConnection->ConnectedDelegate.BindDynamic(this,&AHideAndSeekGameMode::OnTCPConnected);
		}
	}
}
/*
 * 预登录处理函数（临时）
 * 当连接到服务器时，服务器会返回带PlayerInfo的消息，通过Socket反序列化后交由此函数处理
 * 主要是用来设置一下本地的PlayerController
 */
bool AHideAndSeekGameMode::PreLogin(const FBroadCast& BroadCast)
{
	APlayerController* BaseController =  UGameplayStatics::GetPlayerController(this,0); //获取本地的PlayerController
	if (BaseController)
	{
		LocalPC = Cast<AAHASPlayerController>(BaseController); //Cast to AAHASPlayerController
		if (LocalPC)
		{
			LocalPC->SetPlayerInfo(BroadCast.Info);   //设置玩家信息
			LocalPC->IsNetwork = false;               //设置网络状态，这个表示不是远程玩家的pc，是本地的
			PCMap.Add(BroadCast.Info.UserID,LocalPC); //将本地PC放到PC的Map中，以便后续网络同步
			return SocketConnection->SendBroadcastToServer(EMessageType::SyncPlayer,BroadCast); //直接把自己的信息返回给服务器，并且请求同步玩家
		}
		return false;
	}
	return false;
}


/*
 * 同步玩家的函数
 * 主要是将远程的玩家通过服务器，显示在自己的游戏世界中，此函数只在玩家进入游戏时调用，其他时候不调用
 * 一般在Prelogin之后，服务器会根据请求，返回SyncPlayer类型的消息，然后会调用此函数来同步玩家
 */
bool AHideAndSeekGameMode::SyncPlayer(const FBroadCast& BroadCast)
{
	UWorld* const World = GetWorld(); //获取世界
	if (!PCMap.Contains(BroadCast.Info.UserID)) //判断所查找的ID对应的PC是否存在，如果不存在则新建PC,然后生成玩家
	{
		//生成远程玩家的PC
		AAHASPlayerController* NetworkPC= World->SpawnActor<AAHASPlayerController>(AAHASPlayerController::StaticClass(),FVector(0.f,0.f,0.f),FRotator(0.f,0.f,0.f));
		if (NetworkPC)
		{
			//生成角色
			NetworkPC->SpawnCharacter(static_cast<ECharacterType>(BroadCast.Info.CharacterType),BroadCast.Movement.Transform,BroadCast.Info,true);
			NetworkPC->Info = BroadCast.Info;	//设置PC的信息
			NetworkPC->IsNetwork = true;		//设置网络状态
			PCMap.Add(BroadCast.Info.UserID,NetworkPC); //添加PC到PCMap中
			return true;
		}
		return false;
	}
	else
	{
		//如果已经存在了，那么应该只有一种情况，那就是本地的PC，直接生成角色
		LocalPC->SpawnCharacter(static_cast<ECharacterType>(BroadCast.Info.CharacterType),BroadCast.Movement.Transform,BroadCast.Info,false);
	}
	return false;
}


/*
 * 广播函数
 * 将广播消息设置进PC或者角色蓝图中
 */
bool AHideAndSeekGameMode::Broadcast(const FBroadCast& BroadCast)
{
	if (PCMap.Contains(BroadCast.Info.UserID)) //查找ID对应的PCMap是否存在
	{
		AAHASPlayerController* PC = PCMap[BroadCast.Info.UserID]; 
		if (PC)
		{
			PC->Broadcast(BroadCast); //广播
			return true;
		}
		return false;
	}
	return false;
}

/*
 * 踢出玩家
 * 这个函数一般在玩家退出，或者玩家被封禁时调用
 */
bool AHideAndSeekGameMode::KickPlayer(const FBroadCast& BroadCast)
{
	if (PCMap.Contains(BroadCast.Info.UserID)) //查找PCMap中是否存在对应的PC
	{
		if (AAHASPlayerController* KickedPC = PCMap[BroadCast.Info.UserID])
		{
			PCMap.Remove(BroadCast.Info.UserID); //先把对应的PC从PCMap中移除
			KickedPC->SpawnedPawn->Destroy();    //销毁Pawn
			KickedPC->Destroy();				//销毁PC
			return true;
		}
		return false;
	}
	return false;
}



//tick函数，每tick读取本地Pawn中的Broadcast信息，然后将其发送到服务器上
void AHideAndSeekGameMode::Tick(float DeltaSeconds)
{
	/*
	if (LocalPC) //本地PC是否存在
	{
		if (LocalPC->SpawnedPawn)  //PC控制的Pawn是否存在
		{
			IPawnInterface* Implementer = Cast<IPawnInterface>(LocalPC->SpawnedPawn); //将其转换为接口，以便调用函数
			if (Implementer)
			{
				//发送到服务器
				SocketConnection->SendBroadcastToServer(EMessageType::BroadCast,Implementer->GetBroadcast());
			}
		}
	}
	*/
}


UJsonSubsystem* AHideAndSeekGameMode::GetJsonSubsystem()
{
	UGameInstance *Instance = UGameplayStatics::GetGameInstance(GetWorld());
	if (Instance)
	{
		return Instance->GetSubsystem<UJsonSubsystem>();
	}
	return nullptr;
}

void AHideAndSeekGameMode::testfunc(const int& hello, const int& world)
{

}

void AHideAndSeekGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}



