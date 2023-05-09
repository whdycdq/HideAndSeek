// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProtobufLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "HASActorBase.generated.h"
UCLASS()
class AHASActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHASActorBase();
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UBoxComponent* BoxComponent;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Actorname;
	UFUNCTION(BlueprintCallable)
	bool GetBoxCollisionCorner(TArray<FVector>& Output);
	UFUNCTION(BlueprintCallable)
	bool GetBoxAABB(FAABB& AABB);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
