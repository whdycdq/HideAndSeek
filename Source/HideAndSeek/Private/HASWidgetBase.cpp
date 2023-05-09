// Fill out your copyright notice in the Description page of Project Settings.


#include "HASWidgetBase.h"

void UHASWidgetBase::HideUI()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UHASWidgetBase::ShowUI()
{
	this->SetVisibility(ESlateVisibility::Visible);
}

void UHASWidgetBase::Refresh()
{
}
