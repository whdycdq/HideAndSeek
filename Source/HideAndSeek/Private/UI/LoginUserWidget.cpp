// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoginUserWidget.h"
#include "Kismet/GameplayStatics.h"

ULoginUserWidget::ULoginUserWidget()
{
	Bus = UHASDelegateBus::GetInstance();
}

void ULoginUserWidget::UserLogin(FString Username, FString Password)
{
	UHttpSubsystem* Sub = GetHttpSubsystem();
	if (Sub)
	{
		Sub->UserLogin(Username,Password);
	}
}

void ULoginUserWidget::UserRegister(FString Username, FString Password)
{
	UHttpSubsystem* Sub = GetHttpSubsystem();
	if (Sub)
	{
		Sub->UserRegister(Username,Password);
	}
}

void ULoginUserWidget::UserUpdatePassword(FString Username, FString OldPassword, FString NewPassword)
{
	UHttpSubsystem* Sub = GetHttpSubsystem();
	if (Sub)
	{
		Sub->UserUpdatePassword(Username,OldPassword,NewPassword);
	}
}

void ULoginUserWidget::HideUI()
{
	Super::HideUI();
}

void ULoginUserWidget::ShowUI()
{
	Super::ShowUI();
}


UHttpSubsystem* ULoginUserWidget::GetHttpSubsystem()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UHttpSubsystem>();
	}
	return nullptr;
}
