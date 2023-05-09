// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/PawnFactory/HASCharacterFactory.h"

#include "Kismet/GameplayStatics.h"
#include "Pawn/BlueprintReader/CharacterJsonSubsystem.h"
#include "UObject/ConstructorHelpers.h"

UHASCharacterFactory::UHASCharacterFactory():UPawnFactory()
{
	
}

AHASCharacter* UHASCharacterFactory::CreateHASCharacter(UWorld* World,ECharacterType Type,const FTransform& Trans)
{
	if (World)
	{
		UGameInstance* Instance = UGameplayStatics::GetGameInstance(World);
		UCharacterJsonSubsystem* Sub = Instance->GetSubsystem<UCharacterJsonSubsystem>();
		if (Sub)
		{
			if(Sub->CharacterFileMap.Contains(Type))
            			{
            				FString OutputPath = Sub->CharacterFileMap[Type];
            				TSubclassOf<AHASCharacter> CharacterClass = LoadClass<AHASCharacter>(nullptr,*OutputPath);
            				//ConstructorHelpers::FClassFinder<AHASCharacter> BlueprintClassFinder(TEXT("/Game/NewBlueprint"));
            				if (CharacterClass)
            				{
            					FActorSpawnParameters SpawnParameters;
            					SpawnParameters.bNoFail = true;
            					AHASCharacter* Character = World->SpawnActor<AHASCharacter>(CharacterClass,Trans,SpawnParameters);
            					if (Character)
            					{
            						Character->StartLocation = Trans.GetLocation();
            						return Character;
            					}
            					return nullptr;
            				}
            				return nullptr;
            			}
            		return nullptr;
            	}
            	return nullptr;
		}
	return nullptr;
			
}
