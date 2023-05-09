// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProtobufLibrary.h"
#include "UObject/NoExportTypes.h"
#include "PawnJsonReader.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UPawnJsonReader : public UObject
{
	GENERATED_BODY()
public:
	bool Reload();
	bool ReloadCharacter();
	bool ReloadPawn();
	static UPawnJsonReader* GetInstance();
	FString FilePath = FPaths::ProjectContentDir() +TEXT("Json/pawn.json");
	TMap<EPawnType,FString> PawnFileMap;
	TMap<ECharacterType,FString> CharacterFileMap;
private:
	UPawnJsonReader();
	static UPawnJsonReader* Instance;
};
