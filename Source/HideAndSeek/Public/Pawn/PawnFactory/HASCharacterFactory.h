// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/PawnFactory/PawnFactory.h"
#include "HASCharacterFactory.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHASCharacterFactory : public UPawnFactory
{
	GENERATED_BODY()
public:
	UHASCharacterFactory();
	virtual AHASCharacter* CreateHASCharacter(UWorld* World,ECharacterType Type,const FTransform& Trans) override;
};
