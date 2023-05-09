// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/PawnFactory/HASPawnFactory.h"

#include "UObject/ConstructorHelpers.h"

UHASPawnFactory::UHASPawnFactory()
{
	
}

AHASPawn* UHASPawnFactory::CreateHASPawn(UWorld* World,EPawnType Type,const FTransform& Trans)
{
	if (World)
	{
		if(this->PawnJsonReader->PawnFileMap.Contains(Type))
		{
			FString OutputPath = PawnJsonReader->PawnFileMap[Type];
			TSubclassOf<AHASPawn> PawnClass = LoadClass<AHASPawn>(nullptr,*OutputPath);
			if (PawnClass)
			{
				return World->SpawnActor<AHASPawn>(PawnClass,Trans);
			}
			return nullptr;
		}
		return nullptr;
	}
	return nullptr;
}
