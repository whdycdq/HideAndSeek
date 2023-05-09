// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/PawnFactory/PawnJsonReader.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
UPawnJsonReader* UPawnJsonReader::Instance =nullptr;

UPawnJsonReader::UPawnJsonReader()
{
	Reload();
}



bool UPawnJsonReader::Reload()
{
	if (ReloadCharacter()&&ReloadPawn())
	{
		return true;
	}
	return false;
}

bool UPawnJsonReader::ReloadCharacter()
{
	FString JsonString;
	UE_LOG(LogTemp, Warning, TEXT("File path: %s"), *FilePath);
	if (FPaths::FileExists(FilePath))
	{
		FFileHelper::LoadFileToString(JsonString,*FilePath);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		if (FJsonSerializer::Deserialize(JsonReader,JsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField("Character");
			for (auto Value:JsonArray)
			{
				TSharedPtr<FJsonObject> JsonObj = Value->AsObject();
				FString Location = JsonObj->GetStringField("Location");
				ECharacterType CharacterType = static_cast<ECharacterType>(JsonObj->GetIntegerField("Type"));
				CharacterFileMap.Add(CharacterType,Location);
			}
			return true;
		}
		return false;
	}
	return false;
}

bool UPawnJsonReader::ReloadPawn()
{
	FString JsonString;
	if (FPaths::FileExists(FilePath))
	{
		FFileHelper::LoadFileToString(JsonString,*FilePath);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		if (FJsonSerializer::Deserialize(JsonReader,JsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField("Pawn");
			for (auto Value:JsonArray)
			{
				TSharedPtr<FJsonObject> JsonObj = Value->AsObject();
				FString Location = JsonObj->GetStringField("Location");
				EPawnType CharacterType = static_cast<EPawnType>(JsonObj->GetIntegerField("Type"));
				PawnFileMap.Add(CharacterType,Location);
			}
			return true;
		}
		return false;
	}
	return false;
}

UPawnJsonReader* UPawnJsonReader::GetInstance()
{
	if (Instance==nullptr)
	{
		Instance = NewObject<UPawnJsonReader>(); 
	}
	return Instance;
}

