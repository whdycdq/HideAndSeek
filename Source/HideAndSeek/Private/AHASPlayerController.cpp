// Fill out your copyright notice in the Description page of Project Settings.


#include "AHASPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Pawn/HASCharacter.h"
#include "Pawn/Interface/PawnInterface.h"

class UCharacterJsonSubsystem;

AAHASPlayerController::AAHASPlayerController()
{
	UWorld*	World = GetWorld();
	if (World)
	{
		UGameInstance* Instance = UGameplayStatics::GetGameInstance(World);
		if (Instance)
        	{
        		Sub = Instance->GetSubsystem<UCharacterJsonSubsystem>();
        	}
	}
	
}



void AAHASPlayerController::SetPlayerInfo(const FPlayerInfo& PlayerInfo)
{
	this->Info.Token=PlayerInfo.Token;
	this->Info.PlayerName=PlayerInfo.PlayerName;
	this->Info.UserID = PlayerInfo.UserID;
	this->BroadCast.Info.UserID=PlayerInfo.UserID;
	this->BroadCast.Info.PlayerName = PlayerInfo.PlayerName;
	this->BroadCast.Info.Token = PlayerInfo.Token;
}

bool AAHASPlayerController::Broadcast(const FBroadCast& BroadcastInput)
{
	if (IsNetwork)
	{
		IPawnInterface* Implementer = Cast<IPawnInterface>(SpawnedPawn);
		if (Implementer)
		{
			Implementer->SetPawnBehavior(BroadcastInput);
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("Can't Cast pawn to interface!!"));
		return false;
	}
	return false;
}

bool AAHASPlayerController::SpawnCharacter(ECharacterType Type,const FTransform& SpawnTrans,const FPlayerInfo& PlayerInfo,bool network)
{
	UWorld* World = GetWorld();
	if (World&&Sub)
	{
		if (Sub->CharacterFileMap.Contains(Type))
		{
			FString OutputPath = Sub->CharacterFileMap[Type];
			TSubclassOf<AHASCharacter> CharacterClass = LoadClass<AHASCharacter>(nullptr,*OutputPath);
			//ConstructorHelpers::FClassFinder<AHASCharacter> BlueprintClassFinder(TEXT("/Game/NewBlueprint"));
			if (CharacterClass)
			{
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.bNoFail = true;
				AHASCharacter* SpawnedCharacter = World->SpawnActor<AHASCharacter>(CharacterClass,SpawnTrans,SpawnParameters);
				if (SpawnedCharacter)
				{
					//临时性措施，后续会创建接口来实现这个功能
					SpawnedCharacter->StartLocation = SpawnTrans.GetLocation();
					SpawnedPawn = SpawnedCharacter;
					IPawnInterface* Implementer = Cast<IPawnInterface>(SpawnedPawn);
					Implementer->SetIsNetwork(network);
					Implementer->SetPlayerInfo(PlayerInfo);
					this->Possess(SpawnedPawn);
					return true;
				}
				return false;
			}
			return false;
		}
		return false;
	}
	return false;
}

bool AAHASPlayerController::SpawnPawn(EPawnType Type, const FTransform& SpawnTrans, bool network)
{
	return true;
}

void AAHASPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}
