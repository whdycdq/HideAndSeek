// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DownloadSubsystem.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EHttpStatus:uint8
{
	OK = 0 UMETA(DisplayName ="Ok"),
	Error = 1 UMETA(DisplayName ="Error")
};


USTRUCT(BlueprintType)
struct FHttpDownloadInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Url;
	UPROPERTY(BlueprintReadWrite)
	FString FilePath;
	UPROPERTY(BlueprintReadWrite)
	int32 FileLenth;
	UPROPERTY(BlueprintReadWrite)
	int32 ChunkSize;
};

USTRUCT()
struct FDownloadData
{
	GENERATED_BODY()
	int32 StartPosition;
	TArray<uint8> Data;
};

USTRUCT()
struct FOneChunkInfo
{
	GENERATED_BODY()
	int32 StartPosition;
	bool bFinalContent;
	int32 Lenth;
};

#define MAX_PROCESS 5
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnProcessDownload,float,Process);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnProcessFile,float,Process);
DECLARE_DYNAMIC_DELEGATE(FOnFileCompelete);
DECLARE_DYNAMIC_DELEGATE(FOnDownloadCompelete);
DECLARE_DELEGATE_OneParam(FStartDownload,const FOneChunkInfo&);
DECLARE_DELEGATE(FOnEnqueueData);
DECLARE_DELEGATE(FDownloadCompelete);
DECLARE_DELEGATE(FOnFilePreAllocCompelete);
DECLARE_DELEGATE_ThreeParams(FOnOneWorkCompelete,int32,EHttpStatus,const FOneChunkInfo&);


UCLASS()
class HTTPDOWNLOAD_API UDownloadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue;
	static FDownloadCompelete DownloadCompelete;
	static FOnEnqueueData OnEnqueueData;
	UFUNCTION(BlueprintCallable)
	void Download(FString Url,FString FilePath,int32 ChunkSize,const FOnProcessDownload& ProcessDownloadDelegate,const FOnProcessFile& ProcessFileDelegate,const FOnFileCompelete& FileCompeleteDelegate,const FOnDownloadCompelete& DownloadCompeleteDelegate);
private:
	FHttpDownloadInfo HttpDownloadInfo;
	FOnProcessDownload OnProcessDownload;
	FOnProcessFile OnProcessFile;
	FOnFileCompelete OnFileCompelete;
	FOnDownloadCompelete OnDownloadCompelete;
	FOnFilePreAllocCompelete OnFilePreAllocCompelete;
	void OnPreDownloadCompelete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void StartDownload(const FHttpDownloadInfo& Info);
	void PreAllocFile(const FHttpDownloadInfo& Info);
};

class DownloadObject
{
public:
	DownloadObject(int32 Id,FString Url,int32 ChunkSize,TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue);
	FStartDownload StartDownload;
	void StartOneWork(const FOneChunkInfo& ChunkInfo);
private:
	int32 Id;
	FString Url;
	int32 ChunkSize;
	FOneChunkInfo ChunkInfoLocal;
	TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue;
	void OnOneDownloadComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};

class FHTTPDownloadWorker:public FRunnable
{
public:
	FHTTPDownloadWorker(FString Url,int32 FileLenth,int32 ChunkSize,TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue,const FOnDownloadCompelete& DownloadCompeleteDelegate,const FOnProcessDownload& OnProcessDownload);
	static FOnOneWorkCompelete OnOneWorkCompelete;
	virtual ~FHTTPDownloadWorker();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop () override;
	virtual void Exit() override;
private:
	TArray<TUniquePtr<DownloadObject>> ObjectPool;
	TQueue<FOneChunkInfo> ChunkInfoQueue;
	FString Url;
	int32 ChunkSize;
	int32 FileLenth;
	int32 PositionNow = 0;
	int32 Counter = 0;
	bool bDownloadComplete = false;
	bool bOneWorkComplete = false;
	bool DoOnce = false;
	bool ThreadExit = false;
	TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue;
	FOnDownloadCompelete DownloadCompeleteDelegate;
	FOnProcessDownload OnProcessDownload;
	void ApplyForDownload(FOneChunkInfo& ChunkInfo);
	void AssignWork(int32 Id,EHttpStatus Status,const FOneChunkInfo& ChunkInfo);
};

class FHTTPFileWorker:public FRunnable
{
public:
	FHTTPFileWorker(FString FilePath,int32 FileLenth,int32 ChunkSize,TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue,const FOnProcessFile& OnProcessFile,const FOnFileCompelete& OnFileCompelete);
	~FHTTPFileWorker();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop () override;
	virtual void Exit() override;
private:
	FString FilePath;
	int32 FileLenth;
	int32 ChunkSize;
	int32 MaxPosition;
	bool bDownloadCompelete = false;
	FThreadSafeCounter QueueDataNum;
	FOnProcessFile OnProcessFile;
	FOnFileCompelete OnFileCompelete;
	IFileHandle* OutputFileHandle;
	IFileHandle* MD5FileHandle;
	TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue;

	void UpdatePercent(const FDownloadData& Data);
	void AddQueueDataNum();
	void ChangeDownloadCompeleteStatus();

};


class FHttpFileAllocator:public FRunnable
{
public:
	FHttpFileAllocator( FString FilePath, uint64 FileSize,int32 ChunkSize,const FOnFilePreAllocCompelete& PreAllocDelegate);
	virtual ~FHttpFileAllocator();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
private:
	FString FilePath;
	uint64 FileSize;
	int32 ChunkSize;
	FThreadSafeBool bIsStopping;
	FThreadSafeBool bIsFinished;
	FThreadSafeCounter NumBlocksWritten;
	FOnFilePreAllocCompelete PreAllocDelegate;
};
