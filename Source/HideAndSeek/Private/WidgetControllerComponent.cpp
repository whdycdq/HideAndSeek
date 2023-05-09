// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetControllerComponent.h"

#include "Kismet/GameplayStatics.h"
#include "UI/HASChatWidge.h"


// Sets default values for this component's properties
UWidgetControllerComponent::UWidgetControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	// ...
}


// Called when the game starts
void UWidgetControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateChatUI();
	CreateMenuUI();
	CreateLoginUI();
	// ...
	
}

UJsonSubsystem* UWidgetControllerComponent::GetJsonSubsystem()
{
	UGameInstance *Instance = UGameplayStatics::GetGameInstance(GetWorld());
	if (Instance)
	{
		return Instance->GetSubsystem<UJsonSubsystem>();
	}
	return nullptr;
}

void UWidgetControllerComponent::RefreshAllWidget()
{
	
}

void UWidgetControllerComponent::CreateChatUI()
{
	UJsonSubsystem* JsonSubsystem = GetJsonSubsystem();
	if (JsonSubsystem)
	{
		FString Path;
		if(JsonSubsystem->GetWidgetPath("Chat",Path))
		{
			TSubclassOf<UHASChatWidge> WidgetClass = LoadClass<UHASChatWidge>(nullptr,*Path);
			if (WidgetClass)
			{
				if (UHASWidgetBase* Chat = CreateWidget<UHASChatWidge>(GetWorld(),WidgetClass))
				{
					Chat->AddToViewport();
					//Chat->HideUI();	
					WidgetMap.Add(WidgetClass,Chat);
				}
			}
		}
	}
}

void UWidgetControllerComponent::CreateMenuUI()
{
	UJsonSubsystem* JsonSubsystem = GetJsonSubsystem();
	if (JsonSubsystem)
	{
		FString Path;
		if (JsonSubsystem->GetWidgetPath("Menu",Path))
		{
			if (TSubclassOf<UHASWidgetBase> WidgetClass = LoadClass<UHASWidgetBase>(nullptr,*Path))
			{
				if (UHASWidgetBase* Menu = CreateWidget<UHASWidgetBase>(GetWorld(),WidgetClass))
				{	
					Menu->HideUI();	
					WidgetMap.Add(WidgetClass,Menu);
				}
			}
		}
	}
}

void UWidgetControllerComponent::CreateLoginUI()
{
	UJsonSubsystem* JsonSubsystem = GetJsonSubsystem();
	if (JsonSubsystem)
	{
		FString Path;
		if (JsonSubsystem->GetWidgetPath("Login",Path))
		{
			if (TSubclassOf<ULoginUserWidget> WidgetClass = LoadClass<ULoginUserWidget>(nullptr,*Path))
			{
				if (UHASWidgetBase* Login = CreateWidget<ULoginUserWidget>(GetWorld(),WidgetClass))
				{
					Login->AddToViewport();
					Login->SetVisibility(ESlateVisibility::Visible);
					//Login->HideUI();
					WidgetMap.Add(WidgetClass,Login);
				}
			}
		}
	}
}

ULoginUserWidget* UWidgetControllerComponent::GetLoginWidget()
{
	UJsonSubsystem* JsonSubsystem = GetJsonSubsystem();
	if (JsonSubsystem)
	{
		FString Path;
		if(JsonSubsystem->GetWidgetPath("Login",Path))
		{
			TSubclassOf<ULoginUserWidget> WidgetClass = LoadClass<ULoginUserWidget>(nullptr,*Path);
			if (WidgetMap.Contains(WidgetClass))
			{
				if (ULoginUserWidget* ReturnWidget = static_cast<ULoginUserWidget*>(WidgetMap[WidgetClass]))
				{
					return ReturnWidget;
				}
				return nullptr;
			}
			return nullptr;
		}
		return nullptr;
	}
	return nullptr;
}

UHASChatWidge* UWidgetControllerComponent::GetChatWidget()
{
	UJsonSubsystem* JsonSubsystem = GetJsonSubsystem();
	if (JsonSubsystem)
	{
		FString Path;
		if(JsonSubsystem->GetWidgetPath("Chat",Path))
		{
			TSubclassOf<UHASChatWidge> WidgetClass = LoadClass<UHASChatWidge>(nullptr,*Path);
			if (WidgetMap.Contains(WidgetClass))
			{
				if (UHASChatWidge* ReturnWidget = static_cast<UHASChatWidge*>(WidgetMap[WidgetClass]))
				{
					return ReturnWidget;
				}
				return nullptr;
			}
			return nullptr;
		}
		return nullptr;
	}
	return nullptr;
}

void UWidgetControllerComponent::SetPlayerWidget(TArray<FPlayerInfo>& PlayerInfo)
{
	UHASChatWidge* ChatWidget = GetChatWidget();
	if (ChatWidget)
	{
		for (auto it_playerinfo = PlayerInfo.begin();it_playerinfo!=PlayerInfo.end();++it_playerinfo)
		{
			ChatWidget->AddPlayer(*it_playerinfo);
		}
	}
}


// Called every frame
void UWidgetControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

