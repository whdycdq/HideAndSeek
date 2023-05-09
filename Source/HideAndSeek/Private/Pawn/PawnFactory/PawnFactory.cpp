// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/PawnFactory/PawnFactory.h"

#include "Kismet/GameplayStatics.h"
#include "Pawn/PawnFactory/PawnJsonReader.h"

UPawnFactory::UPawnFactory()
{

}

AHASPawn* UPawnFactory::CreateHASPawn(UWorld* World,EPawnType Type,const FTransform& Trans)
{
	return nullptr;
}

AHASCharacter* UPawnFactory::CreateHASCharacter(UWorld* World,ECharacterType Type,const FTransform& Trans)
{
	return nullptr;
}
