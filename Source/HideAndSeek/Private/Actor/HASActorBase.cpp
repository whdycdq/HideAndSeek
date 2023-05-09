// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HASActorBase.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHASActorBase::AHASActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(SceneComponent);
}

bool AHASActorBase::GetBoxCollisionCorner(TArray<FVector>& Output)
{
	if (BoxComponent)
	{
		FVector BoxMin = BoxComponent->Bounds.Origin - BoxComponent->Bounds.BoxExtent;
		FVector BoxMax = BoxComponent->Bounds.Origin + BoxComponent->Bounds.BoxExtent;
		Output.Add(BoxMin);
		Output.Add(BoxMax);
		return true;
	}
	return false;
}

bool AHASActorBase::GetBoxAABB(FAABB& AABB)
{
	if (BoxComponent)
	{
		AABB.Min = BoxComponent->Bounds.Origin - BoxComponent->Bounds.BoxExtent;
		AABB.Max = BoxComponent->Bounds.Origin + BoxComponent->Bounds.BoxExtent;
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
void AHASActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHASActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

