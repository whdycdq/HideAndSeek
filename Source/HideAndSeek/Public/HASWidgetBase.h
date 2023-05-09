// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HASWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class HIDEANDSEEK_API UHASWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void HideUI();
	UFUNCTION(BlueprintCallable)
	virtual void ShowUI();
	UFUNCTION(BlueprintCallable)
	virtual void Refresh();
};
