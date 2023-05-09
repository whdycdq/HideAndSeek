// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "HASDelegateBus.h"
#include "HttpModule.h"
#include "ProtobufLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestDownload,int32&,Position,bool&,bFinalContent,int32&,Lenth);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnProcessDownload,float,Process);
DECLARE_DYNAMIC_DELEGATE(FDownloadCompelete);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnProcessFile,float,Process);
DECLARE_DYNAMIC_DELEGATE(FOnFileCompelete);
DECLARE_DELEGATE(FOnFilePreAllocCompelete);
DECLARE_DELEGATE(FOnDownloadCompelete);
DECLARE_DELEGATE(FOnEnqueueData);

USTRUCT(BlueprintType)
struct FDownloadFileRequire
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Version;
	UPROPERTY(BlueprintReadWrite)
	FString Filename;
	UPROPERTY(BlueprintReadWrite)
	FString Url;
	UPROPERTY(BlueprintReadWrite)
	FString MD5;
};


USTRUCT(BlueprintType)
struct FDownloadInfo
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
	UPROPERTY(BlueprintReadWrite)
	int32 ThreadNum;
	
};

USTRUCT(BlueprintType)
struct FDownloadChunkInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int32 StartPosition;
	UPROPERTY(BlueprintReadWrite)
	int32 ChunkNum;
	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> Data;
};


class FFilePreAllocator : public FRunnable
{
public:
    FFilePreAllocator(const FString& FilePath, uint64 FileSize,int32 ChunkSize,const FOnFilePreAllocCompelete& PreAllocDelegate)
	: FilePath(FilePath)
	,FileSize(FileSize)
	,ChunkSize(ChunkSize)
	,bIsStopping(false)
	,bIsFinished(false)
	,NumBlocksWritten(0)
	,PreAllocDelegate(PreAllocDelegate)
	{
	};

    virtual ~FFilePreAllocator();

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


class FDownloadWorker;
UCLASS()
class HIDEANDSEEK_API UHttpSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UHttpSubsystem();
	TArray<FRunnableThread*> RunningDownloadThreads;
	TMap<int32,FDownloadWorker*> DownloadWorkers;
	TQueue<FDownloadChunkInfo,EQueueMode::Mpsc> DataQueue;
	UHASDelegateBus* Bus;
	TArray<uint8> Content;
	FString URL;
	FString SavePath;
	FOnFilePreAllocCompelete PreAllocDelegate;
	FOnDownloadCompelete DownloadCompeleteDelegate;
	FOnProcessDownload OnProcessDownload;
	FDownloadCompelete DownloadCompelete;
	FOnProcessFile OnProcessFile;
	FOnFileCompelete OnFileCompelete;
	UPROPERTY(BlueprintReadWrite)
	FString ServerBaseUrl = "http://127.0.0.1:8080/";
	UFUNCTION(BlueprintCallable)
	void UserLogin(FString Username, FString Password);
	UFUNCTION(BlueprintCallable)
	void UserRegister(FString Username, FString Password);
	UFUNCTION(BlueprintCallable)
	void UserUpdatePassword(FString Username, FString OldPassword,FString NewPassword);
	void StartDownloadThread();
	void StartDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UFUNCTION(BlueprintCallable)
	void DownloadSync(FString Url,FString FilePath,int32 ChunkSize,int32 ThreadNum,const FOnProcessDownload& OnProcessDownloadDelegate,const FDownloadCompelete& OnDownloadCompeleteDelegate,const FOnProcessFile& OnProcessFileDelegate,const FOnFileCompelete& OnFileCompeleteDelegate);
	UFUNCTION(BlueprintCallable)
	void DownloadMultiFile(TArray<FDownloadFileRequire>&FileArray,int32 ChunkSize,int32 ThreadNum,const FOnProcessDownload& OnProcessDownloadDelegate,const FDownloadCompelete& OnDownloadCompeleteDelegate,const FOnProcessFile& OnProcessFileDelegate,const FOnFileCompelete& OnFileCompeleteDelegate);
	UFUNCTION(BlueprintCallable)
	void SetDownloadPath(FString Path);
	UFUNCTION(BlueprintCallable)
	void SetDownloadUrl(FString Url);
	void ProcessOnRequestDownload(int32& Position,bool& bFinalContent,int32& Lenth);
	UFUNCTION(BlueprintCallable)
	bool UnmarshalVersionJson(const FString& Json,TArray<FDownloadFileRequire>&FileArray);
	UFUNCTION(BlueprintCallable)
	void PreAllocFile(const FString& FilePath, int32 FileSize,int32 ChunkSize);
	void OnPreAllocCompelete();
	//下载完成调用
	void HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	//处理下载进度
	void HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);
	
	void OnLoginUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnRegisterUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	void OnPasswordUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
private:
	FDownloadInfo DownloadInfo;
	int32 PositionNow;
};



class FFileWorker :public FRunnable,TSharedFromThis<FFileWorker>
{
public:
	FFileWorker(FString FileDir,int32 FileLenth,int32 ChunkSize,TQueue<FDownloadChunkInfo,EQueueMode::Mpsc>& DataQueue,const FOnDownloadCompelete& DownloadCompeleteDelegate,const FOnProcessFile& OnProcessFile,const FOnFileCompelete& OnFileCompelete,const FOnEnqueueData& OnEnqueueData);
	~FFileWorker();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop () override;
	virtual void Exit() override;
	FThreadSafeBool bDownloadCompelete = false;
	FOnEnqueueData OnEnqueueData;
private:
	void OnDownloadComplete();
	bool WriteMD5(FDownloadChunkInfo& Info);
	void UpdatePercent(FDownloadChunkInfo& Info);
	FString FileDir;
	FMD5 Md5Gen;
	TQueue<FDownloadChunkInfo,EQueueMode::Mpsc>& DataQueue;
	FOnDownloadCompelete DownloadCompeleteDelegate;
	FOnProcessFile OnProcessFile;
	FOnFileCompelete OnFileCompelete;
	IFileHandle* OutputFileHandle;
	IFileHandle* MD5FileHandle;
	int32 FileLenth;
	int32 ChunkSize;
	int32 MaxPosition;
	int32 QueueCounter = 0;
	uint8* MD5Array;
};

class FDownloadWorker :public FRunnable,public TSharedFromThis<FDownloadWorker>
{
public:
	FDownloadWorker(FString Url,FString FilePath,int32 ThreadNum,TQueue<FDownloadChunkInfo,EQueueMode::Mpsc>& DataQueue,const FOnDownloadCompelete& DownloadCompeleteDelegate,const FOnProcessDownload& OnProcessDownload,const FDownloadCompelete& DownloadCompelete,const FOnEnqueueData& OnEnqueueData)
	:Url(Url)
	,FilePath(FilePath)
	,ThreadNum(ThreadNum)
	,DataQueue(DataQueue)
	,DownloadCompeleteDelegate(DownloadCompeleteDelegate)
	,OnProcessDownload(OnProcessDownload)
	,DownloadCompelete(DownloadCompelete)
	,OnEnqueueData(OnEnqueueData)
	{
	};
	virtual ~FDownloadWorker();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop () override;
	virtual void Exit() override;
	void ProcessDownload(bool bFinalContent,int32 Lenth);
	static void SetFileLenth(int32 initFileLenth);
	static void SetChunkSize(int32 initChunkSize);
	static void SetPosition(int initSetPosition);
private:
	FString Url;
	FString FilePath;
	int32 StartPosition = 0;
	int32 ThreadNum = 0;
	int32 ChunkNow = 0;
	static int32 ChunkSize;
	static int32 FileLenth;
	static int32 PositionNow;
	static FCriticalSection Mutex;
	static FCriticalSection ExitMutex;
	static FCriticalSection QueueCounterMutex;
	static FThreadSafeCounter NumThreads;
	static FThreadSafeCounter ChunkCounter;
	TQueue<FDownloadChunkInfo,EQueueMode::Mpsc>& DataQueue;
	const FOnRequestDownload OnRequestDownload;
	const FOnProcessDownload OnProcessDownload;
	const FDownloadCompelete DownloadCompelete;
	const FOnEnqueueData OnEnqueueData;
	FThreadSafeBool bExit = false;
	FThreadSafeBool bStartedDownload = false;
	FOnDownloadCompelete DownloadCompeleteDelegate;
	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
	static void ProcessOnRequestDownload(int32& Position,int32& ChunkNum, bool& bFinalContent, int32& Lenth);
	static void AddExitNum();
};
