// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnJsonReader.h"
#include "ProtobufLibrary.h"
#include "Pawn/HASCharacter.h"
#include "Pawn/HASPawn.h"
#include "UObject/NoExportTypes.h"
#include "PawnFactory.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UPawnFactory : public UObject
{
	GENERATED_BODY()
public:
	UPawnFactory();
	UPawnJsonReader* PawnJsonReader;
	virtual AHASPawn* CreateHASPawn(UWorld* World,EPawnType Type,const FTransform& Trans);
	virtual AHASCharacter* CreateHASCharacter(UWorld* World,ECharacterType Type,const FTransform& Trans);
};
