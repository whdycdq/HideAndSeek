// Fill out your copyright notice in the Description page of Project Settings.


#include "Json/JsonSubsystem.h"

UJsonSubsystem::UJsonSubsystem()
{
	Reload();
}

void UJsonSubsystem::Reload()
{
	ReloadWidgetJson();
}


bool UJsonSubsystem::ReloadWidgetJson()
{
	FString JsonString;
	if (FPaths::FileExists(WidgetJsonPath))
	{
		FFileHelper::LoadFileToString(JsonString,*WidgetJsonPath);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
		if (FJsonSerializer::Deserialize(JsonReader,JsonObject))
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField("Widget");
			for (auto Value:JsonArray)
			{
				TSharedPtr<FJsonObject> JsonObj = Value->AsObject();
				FString Location = JsonObj->GetStringField("Location");
				FString Type = JsonObj->GetStringField("Type");
				WidgetMap.Add(Type,Location);
			}
			return true;
		}
		return false;
	}
	return false;
}

bool UJsonSubsystem::GetWidgetPath(FString Name,FString& Path)
{
	if (WidgetMap.Contains(Name))
	{
		Path = WidgetMap[Name];
		return true;
	}
	return false;
}

