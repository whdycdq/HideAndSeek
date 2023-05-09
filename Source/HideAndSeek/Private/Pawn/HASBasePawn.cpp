// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HASBasePawn.h"

// Sets default values
AHASBasePawn::AHASBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AHASBasePawn::SetPawnBehavior(const FBroadCast& InputBroadcast)
{
	
}

// Called when the game starts or when spawned
void AHASBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHASBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHASBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHASBasePawn::UpdateBroadcast()
{
	
}

void AHASBasePawn::Jump()
{
	
}

