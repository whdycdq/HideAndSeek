// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Position.pb.h"
#include "GameFramework/Character.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProtobufLibrary.generated.h"
UENUM(BlueprintType)
enum class EMessageType:uint8
{
	Default = 0 UMETA(DisplayName ="Default"),
	PreLogin = 1 UMETA(DisplayName ="PreLogin"),
	SyncPlayer = 2 UMETA(DisplayName = "SyncPlayer"),
	BroadCast = 200 UMETA(DisplayName ="BroadCast"),
	KickPlayer = 201 UMETA(DisplayName = "KickPlayer"),
	Chat = 202 UMETA(DisplayName = "Chat"),
	RayIntersect = 203 UMETA(DisplayName = "RayIntersect")
};

UENUM()
enum class ECharacterType:uint8
{
	Default = 0 UMETA(DisplayName = "Default"),
	Alice = 1 UMETA(DisplayName = "Alice")
};

UENUM()
enum class EPawnType:uint8
{
	Default = 0 UMETA(DisplayName = "Default")
};

UENUM()
enum class EChatType:uint8
{
	Unicast = 0 UMETA(DisplayName = "Unicast"),
	Multicast = 1 UMETA(DisplayName = "Multicast"),
	Broadcast = 2 UMETA(DisplayName  = "Broadcast")
};
struct MovementStruct
{
	Player::PlayerMovementInfo* NewInfo;
	Player::ControllerRotation* NewCRotator;
	Player::Transform* NewTrans;
	Player::Location* NewLocation;
	Player::Rotation* NewRotator;
	Player::Scale* NewScale;
	Player::Axis* Axis;
	MovementStruct()
	{
		NewInfo = new Player::PlayerMovementInfo;
		NewCRotator = new Player::ControllerRotation;
		NewTrans  = new Player::Transform;
		NewLocation = new Player::Location;
		NewRotator = new Player::Rotation;
		NewScale = new Player::Scale;
		Axis = new Player::Axis;
	}
	~MovementStruct()
	{
		delete NewInfo;
	}
};


struct BroadcastStruct
{
	MovementStruct* MovementStruct;
	Player::BroadCast* Temp;
	Player::SyncPlayerInfo* PlayerInfo;
	Player::SyncPlayerToken* Token;
	
	BroadcastStruct()
	{
		MovementStruct =new struct MovementStruct;
		Temp =  new Player::BroadCast;
		PlayerInfo = new Player::SyncPlayerInfo;
		Token =  new Player::SyncPlayerToken;
	}
	~BroadcastStruct()
	{
		delete Temp;
	}
};




USTRUCT(BlueprintType)
struct FAxisStruct
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	float MoveForward;
	UPROPERTY(BlueprintReadWrite)
	float MoveRight;
};

USTRUCT(BlueprintType)
struct FPlayerMovementInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FAxisStruct Axis;
	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;
	UPROPERTY(BlueprintReadWrite)
	FRotator ControllerRotation;
};
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int32 UserID;
	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;
	UPROPERTY(BlueprintReadWrite)
	FString Token;
	UPROPERTY(BlueprintReadWrite)
	int32 CharacterType;
};
USTRUCT(BlueprintType)
struct FBroadCast
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FPlayerMovementInfo Movement;
	UPROPERTY(BlueprintReadWrite)
	FPlayerInfo Info;
	UPROPERTY(BlueprintReadWrite)
	int32 Content;
	UPROPERTY(BlueprintReadWrite)
	int32 ActionData;
};

USTRUCT(BlueprintType)
struct FChat
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> PlayerArray;
	UPROPERTY(BlueprintReadWrite)
	EChatType Type;
	UPROPERTY(BlueprintReadWrite)
	FString Chat;
};

USTRUCT(BlueprintType)
struct FAABB
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FVector Min;
	UPROPERTY(BlueprintReadWrite)
	FVector Max;
};

USTRUCT(BlueprintType)
struct FRayIntersect
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FVector Origin;
	UPROPERTY(BlueprintReadWrite)
	FVector Direction;
	UPROPERTY(BlueprintReadWrite)
	FString Actor;
};

UCLASS()
class HIDEANDSEEK_API UProtobufLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UProtobufLibrary();
	
	bool MarshalMovementMessage(MovementStruct* MovementStruct,const FPlayerMovementInfo& Info, TArray<uint8>& Output);
	bool UnmarshalMovementMessage(TArray<uint8> Input,FPlayerMovementInfo& Info);
	UFUNCTION(BlueprintCallable)
	static bool UnmarshalToken(const TArray<uint8>& Input, FPlayerInfo& Info);
	UFUNCTION(BlueprintCallable)
	static bool UnmarshalBroadCast(const TArray<uint8>& Input, FBroadCast& Info);
	//UFUNCTION(BlueprintCallable)
	//bool MarshalTokenByLocalController(TArray<uint8>& Output);
	UFUNCTION(BlueprintCallable)
	static bool MarshalBroadCast(const FBroadCast& Info,TArray<uint8>& Output);
	UFUNCTION(BlueprintCallable)
	static bool UnMarshalBroadCastByMessageType(EMessageType Type,const TArray<uint8>& Input, FBroadCast& Info);
	UFUNCTION(BlueprintCallable)
	static bool UnMarshalChat(const TArray<uint8>& Input,FChat& Chat);
	UFUNCTION(BlueprintCallable)
	static bool MarshalChat(const FChat& Chat,TArray<uint8>&Output);
	UFUNCTION(BlueprintCallable)
	static bool MarshalRayIntersect(const FRayIntersect& RayIntersect,TArray<uint8>&Output);
	UFUNCTION(BlueprintCallable)
	static bool UnMarshalRayIntersect(const TArray<uint8>& Input,FRayIntersect& RayIntersect);
};

