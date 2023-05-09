// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HASWidgetBase.h"
#include "Components/ActorComponent.h"
#include "Json/JsonSubsystem.h"
#include "UI/HASChatWidge.h"
#include "UI/LoginUserWidget.h"
#include "WidgetControllerComponent.generated.h"


USTRUCT()
struct FMyStruct
{
	GENERATED_BODY()
	TArray<int> test;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HIDEANDSEEK_API UWidgetControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this component's properties
	UWidgetControllerComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<TSubclassOf<UHASWidgetBase>,UHASWidgetBase*> WidgetMap;
	UFUNCTION(BlueprintCallable)
	UJsonSubsystem* GetJsonSubsystem();
	UFUNCTION(BlueprintCallable)
	void RefreshAllWidget();
	UFUNCTION(BlueprintCallable)
	void CreateChatUI();
	UFUNCTION(BlueprintCallable)
	void CreateMenuUI();
	UFUNCTION(BlueprintCallable)
	void CreateLoginUI();
	UFUNCTION(BlueprintCallable)
	ULoginUserWidget* GetLoginWidget();
	UFUNCTION(BlueprintCallable)
	UHASChatWidge* GetChatWidget();
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerWidget(TArray<FPlayerInfo>& PlayerInfo);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
