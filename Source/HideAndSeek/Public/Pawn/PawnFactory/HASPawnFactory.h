// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/PawnFactory/PawnFactory.h"
#include "HASPawnFactory.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHASPawnFactory : public UPawnFactory
{
	GENERATED_BODY()
public:
	UHASPawnFactory();
	virtual AHASPawn* CreateHASPawn(UWorld* World,EPawnType Type,const FTransform& Trans) override;
};
