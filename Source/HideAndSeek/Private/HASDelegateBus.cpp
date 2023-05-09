// Fill out your copyright notice in the Description page of Project Settings.


#include "HASDelegateBus.h"

UHASDelegateBus* UHASDelegateBus::HASDelegateBusPtr = nullptr;

UHASDelegateBus* UHASDelegateBus::GetInstance()
{
	if (HASDelegateBusPtr==nullptr)
	{
		HASDelegateBusPtr = NewObject<UHASDelegateBus>();
		return HASDelegateBusPtr;
	}
	return HASDelegateBusPtr;
}

UHASDelegateBus::UHASDelegateBus()
{
	
}


