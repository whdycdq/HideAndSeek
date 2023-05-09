// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HASWidgetBase.h"
#include "ProtobufLibrary.h"
#include "HASDelegateBus.h"
#include "Http/HttpSubsystem.h"
#include "LoginUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API ULoginUserWidget : public UHASWidgetBase
{
	GENERATED_BODY()
public:
	ULoginUserWidget();
	UHASDelegateBus* Bus;
	UPROPERTY(BlueprintReadWrite)
	FString ServerBaseUrl = "http://127.0.0.1:8080/";
	UPROPERTY(BlueprintReadWrite)
	FBroadCast BroadCast;
	UFUNCTION(BlueprintCallable)
	void UserLogin(FString Username, FString Password);
	UFUNCTION(BlueprintCallable)
	void UserRegister(FString Username, FString Password);
	UFUNCTION(BlueprintCallable)
	void UserUpdatePassword(FString Username, FString OldPassword,FString NewPassword);
	virtual void HideUI() override;
	virtual void ShowUI() override;
	UHttpSubsystem* GetHttpSubsystem();
};
