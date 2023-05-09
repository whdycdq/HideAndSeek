// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HASPawn.h"

AHASPawn::AHASPawn()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxComponent);
	Movement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("CharacterMovement"));
	if (Movement)
	{
		Movement->UpdatedComponent=BoxComponent;
		Movement->MaxWalkSpeed = 500.f;
		Movement->JumpZVelocity = 500.f;
	}
}

void AHASPawn::SetPawnBehavior(const FBroadCast& InputBroadcast)
{
	
}

void AHASPawn::Jump()
{
	
}

void AHASPawn::UpdateBroadcast()
{
	
}

void AHASPawn::SetIsNetwork(bool bNetwork)
{
}

bool AHASPawn::GetIsNetwork()
{
	return IsNetwork;
}

void AHASPawn::SetBroadcast(const FBroadCast& bc)
{
}

FBroadCast& AHASPawn::GetBroadcast()
{
	return Broadcast;
}

void AHASPawn::SetPlayerInfo(const FPlayerInfo& PlayerInfo)
{
}

FPlayerInfo& AHASPawn::GetPlayerInfo()
{
	return Info;
}

void AHASPawn::SetAxis(FAxisStruct axis)
{
	this->Axis = axis;
}

FAxisStruct& AHASPawn::GetAxis()
{
	return Axis;	
}
