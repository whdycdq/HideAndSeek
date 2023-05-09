// Copyright Epic Games, Inc. All Rights Reserved.

#include "HttpDownloadBPLibrary.h"
#include "HttpDownload.h"

UHttpDownloadBPLibrary::UHttpDownloadBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UHttpDownloadBPLibrary::HttpDownloadSampleFunction(float Param)
{
	test.Enqueue(1);
	return -1;
}

