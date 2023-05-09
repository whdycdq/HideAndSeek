// Fill out your copyright notice in the Description page of Project Settings.


#include "MapEditorBase.h"
#include "Json.h"
#include "JsonUtilities.h"
TSharedPtr<FJsonObject> AMapEditorBase::ToJsonValue(AHASActorBase* Actor)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> VectorArray;
	TArray<FVector> Vectortemp;
	Actor->GetBoxCollisionCorner(Vectortemp);
	for (const FVector& Vector:Vectortemp)
	{
		VectorArray.Add(MakeShareable(new FJsonValueString(Vector.ToString())));
	}
	JsonObject->SetArrayField("Corner",VectorArray);
	return JsonObject;
}

void AMapEditorBase::SerializeMapInfo(const TArray<AHASActorBase*>& Actor)
{
	FString OutputString;
	TSharedPtr<FJsonObject> rootObject = MakeShareable(new FJsonObject());
	for (AHASActorBase* ActorTemp :Actor)
	{
		TSharedPtr<FJsonObject> JsonObject = ToAABBJsonValue(ActorTemp);
		rootObject->SetObjectField(ActorTemp->Actorname,JsonObject);
	}
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(rootObject.ToSharedRef(),Writer);
	bool WriteJson = FFileHelper::SaveStringToFile(OutputString, *this->Path);
}

TSharedPtr<FJsonObject> AMapEditorBase::ToAABBJsonValue(AHASActorBase* Actor)
{
	FAABB AABB;
	Actor->GetBoxAABB(AABB);
	TSharedPtr<FJsonObject> RootJsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> AABBJsonObject = MakeShareable(new FJsonObject);
	
	// 设置 Min 和 Max 属性的值
	AABBJsonObject->SetStringField("Min", AABB.Min.ToString());
	AABBJsonObject->SetStringField("Max", AABB.Max.ToString());

	// 将 AABBJsonObject 添加到 RootJsonObject 中
	RootJsonObject->SetObjectField("AABB", AABBJsonObject);
	return RootJsonObject;
}

