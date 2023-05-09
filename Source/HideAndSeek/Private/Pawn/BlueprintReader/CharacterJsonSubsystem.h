// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Protobuf/ProtobufLibrary.h"
#include "CharacterJsonSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UCharacterJsonSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	bool Reload();
	bool ReloadCharacter();
	bool ReloadPawn();
	FString FilePath = FPaths::ProjectContentDir() +TEXT("Json/pawn.json");
	TMap<EPawnType,FString> PawnFileMap;
	TMap<ECharacterType,FString> CharacterFileMap;
};
