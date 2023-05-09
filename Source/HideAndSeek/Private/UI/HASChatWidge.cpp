// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HASChatWidge.h"

#include "Kismet/KismetTextLibrary.h"

UHASChatWidge::UHASChatWidge()
{
	Bus = UHASDelegateBus::GetInstance();
}

UHASChatWidge::UHASChatWidge(FString ChildPath)
{
	this->ChildPath = ChildPath;
}


bool UHASChatWidge::SendChat(FText Text, EChatType Type, TArray<int32>& uid)
{

	FChat Chat;
	Chat.Chat = UKismetTextLibrary::Conv_TextToString(Text);
	Chat.Type = Type;
	for (auto it_uid = uid.begin();it_uid!=uid.end(); ++it_uid)
	{
		Chat.PlayerArray.Add(*it_uid);
	}
	if (Bus)
	{
		return Bus->SendChatDelegate.ExecuteIfBound(Chat);	
	}
	return false;
}

void UHASChatWidge::ShowUI()
{
	Super::ShowUI();
}

void UHASChatWidge::HideUI()
{
	Super::HideUI();
}

void UHASChatWidge::Refresh()
{
	Super::Refresh();
}
