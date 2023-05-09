// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HASCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

AHASCharacter::AHASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 60.f;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AHASCharacter::SetPawnBehavior(const FBroadCast& InputBroadcast)
{
	TargetLoction = InputBroadcast.Movement.Transform.GetLocation();
	Speed = GetSpeed(StartLocation,TargetLoction);
	StartLocation = TargetLoction;
	USkeletalMeshComponent* TempMesh = this->GetMesh();
	if (TempMesh)
	{
		TempMesh->SetWorldTransform(InputBroadcast.Movement.Transform);
		if (Broadcast.ActionData==1)
		{
			Jump();
		}
	}
}



void AHASCharacter::UpdateBroadcast()
{
	USkeletalMeshComponent* TempMesh = this->GetMesh();
	if (TempMesh)
	{
		Broadcast.Movement.Transform = TempMesh->GetComponentToWorld();
		Broadcast.Movement.ControllerRotation = GetControlRotation();
		Broadcast.Movement.Axis = Axis;
		Broadcast.Info = Info;
		
		if (IsJump)
		{
			Broadcast.ActionData = 1;
		}
		else
		{
			Broadcast.ActionData = 0;
		}
	}
}

void AHASCharacter::SetIsNetwork(bool bNetwork)
{
	IsNetwork = bNetwork;
}

bool AHASCharacter::GetIsNetwork()
{
	return IsNetwork;
}

void AHASCharacter::SetBroadcast(const FBroadCast& broadCast)
{
	Broadcast=broadCast;
}

FBroadCast& AHASCharacter::GetBroadcast()
{
	return Broadcast;
}

void AHASCharacter::SetPlayerInfo(const FPlayerInfo& PlayerInfo)
{
	this->Info = PlayerInfo;
	this->Broadcast.Info = PlayerInfo;
}

FPlayerInfo& AHASCharacter::GetPlayerInfo()
{
	return Info;
}

void AHASCharacter::SetAxis(const FAxisStruct axis)
{
	this->Axis = axis;
}

FAxisStruct& AHASCharacter::GetAxis()
{
	return Axis;
}

float AHASCharacter::GetSpeed(const FVector& Start, const FVector& Target)
{
	float Sub_X = Target.X-Start.X;
	float Sub_Y = Target.Y-Start.Y;
	float Square_X = UKismetMathLibrary::Square(Sub_X);
	float Square_Y = UKismetMathLibrary::Square(Sub_Y);
	float Square_XY =Square_X+Square_Y;
	float Root_XY = UKismetMathLibrary::Sqrt(Square_XY);
	float tick = 1.f/60.f;
	return Root_XY/tick;
}
void AHASCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHASCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHASCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHASCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHASCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHASCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHASCharacter::OnResetVR);
}

void AHASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsNetwork)
	{
		//每tick更新角色的运动信息
		UpdateBroadcast();
		//通过事件调度器发送给服务器
		UHASDelegateBus::GetInstance()->OnTick.ExecuteIfBound(this->Broadcast);
	}
}


void AHASCharacter::OnResetVR()
{
	// If HideAndSeek is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in HideAndSeek.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
}

void AHASCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AHASCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AHASCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHASCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHASCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHASCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

