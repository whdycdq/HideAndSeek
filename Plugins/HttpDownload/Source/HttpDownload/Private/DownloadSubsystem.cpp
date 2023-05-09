// Fill out your copyright notice in the Description page of Project Settings.


#include "DownloadSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/KismetStringLibrary.h"
FDownloadCompelete UDownloadSubsystem::DownloadCompelete;
FOnEnqueueData UDownloadSubsystem::OnEnqueueData;
FOnOneWorkCompelete FHTTPDownloadWorker::OnOneWorkCompelete;
void UDownloadSubsystem::Download(FString Url,FString FilePath,int32 ChunkSize,const FOnProcessDownload& ProcessDownloadDelegate,const FOnProcessFile& ProcessFileDelegate,const FOnFileCompelete& FileCompeleteDelegate,const FOnDownloadCompelete& DownloadCompeleteDelegate)
{
	DataQueue = MakeShareable(new TQueue<FDownloadData,EQueueMode::Spsc>);
	this->OnProcessDownload = ProcessDownloadDelegate;
	this->OnProcessFile = ProcessFileDelegate;
	this->OnFileCompelete = FileCompeleteDelegate;
	this->OnDownloadCompelete = DownloadCompeleteDelegate;
	HttpDownloadInfo.Url = Url;
	HttpDownloadInfo.FilePath = FilePath;
	HttpDownloadInfo.ChunkSize = ChunkSize;
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->SetVerb("HEAD");
	Request->SetURL(Url);
	Request->OnProcessRequestComplete().BindUObject(this,&UDownloadSubsystem::OnPreDownloadCompelete);
	Request->ProcessRequest();
	
}

void UDownloadSubsystem::OnPreDownloadCompelete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,bool bSucceeded)
{
	if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetContentLength() <= 0)
	{
		return;
	}
	
	else
	{
		this->HttpDownloadInfo.FileLenth = HttpResponse->GetContentLength();
		PreAllocFile(this->HttpDownloadInfo);
	}
}

void UDownloadSubsystem::PreAllocFile(const FHttpDownloadInfo& Info)
{
	OnFilePreAllocCompelete.BindLambda([this](){StartDownload(this->HttpDownloadInfo);});
	FHttpFileAllocator* Allocor = new FHttpFileAllocator(this->HttpDownloadInfo.FilePath,this->HttpDownloadInfo.FileLenth,this->HttpDownloadInfo.ChunkSize,OnFilePreAllocCompelete);
	FRunnableThread::Create(Allocor,TEXT("AllocWorker"));
}

DownloadObject::DownloadObject(int32 Id, FString Url, int32 ChunkSize,TSharedPtr<TQueue<FDownloadData, EQueueMode::Spsc>> DataQueue)
{
	this->Id = Id;
	this->Url = Url;
	this->DataQueue = DataQueue;
	this->ChunkSize = ChunkSize;
	StartDownload.BindRaw(this,&DownloadObject::StartOneWork);
}

void DownloadObject::StartOneWork(const FOneChunkInfo& ChunkInfo)
{
	if (!ChunkInfo.bFinalContent&&ChunkInfo.Lenth!=0)
	{
		const int32 EndPos = ChunkInfo.StartPosition+ChunkSize;
		FString RangeHeader = FString::Printf(TEXT("bytes=%d-%d"), ChunkInfo.StartPosition, EndPos - 1);
		UE_LOG(LogTemp, Warning, TEXT("Thread Download Position : %d  to  %d"), ChunkInfo.StartPosition,EndPos);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb(TEXT("GET"));
		HttpRequest->SetURL(Url);
		HttpRequest->SetHeader(TEXT("Range"), RangeHeader);
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &DownloadObject::OnOneDownloadComplete);
		HttpRequest->ProcessRequest();
		ChunkInfoLocal = ChunkInfo;
	}
	else if (ChunkInfo.bFinalContent&&ChunkInfo.Lenth!=0)
	{
		FString RangeHeader = FString::Printf(TEXT("bytes=%d-%d"), ChunkInfo.StartPosition, ChunkInfo.StartPosition+ChunkInfo.Lenth-1);
		UE_LOG(LogTemp, Warning, TEXT("Thread Download Position : %d  to  %d"), ChunkInfo.StartPosition,ChunkInfo.StartPosition+ChunkInfo.Lenth-1);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb(TEXT("GET"));
		HttpRequest->SetURL(Url);
		HttpRequest->SetHeader(TEXT("Range"), RangeHeader);
		HttpRequest->OnProcessRequestComplete().BindRaw(this,  &DownloadObject::OnOneDownloadComplete);
		HttpRequest->ProcessRequest();
		ChunkInfoLocal = ChunkInfo;
	}
}

void DownloadObject::OnOneDownloadComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetResponseCode() != 206)
	{
		FHTTPDownloadWorker::OnOneWorkCompelete.ExecuteIfBound(Id,EHttpStatus::Error,ChunkInfoLocal);
	}
	else
	{
		FDownloadData Data;
		Data.Data=HttpResponse->GetContent();
		FString EqualLeft;
		FString EqualRight;
		FString SlashLeft;
		FString SlashRight;
		HttpRequest->GetHeader("Range").Split("=",&EqualLeft,&EqualRight);
		EqualRight.Split("-",&SlashLeft,&SlashRight);
		Data.StartPosition = UKismetStringLibrary::Conv_StringToInt(SlashLeft);
		if (DataQueue->Enqueue(Data))
		{
			FHTTPDownloadWorker::OnOneWorkCompelete.ExecuteIfBound(Id,EHttpStatus::OK,ChunkInfoLocal);
		}
	}
}

void UDownloadSubsystem::StartDownload(const FHttpDownloadInfo& Info)
{
	FHTTPFileWorker* FileWorker = new FHTTPFileWorker(Info.FilePath,Info.FileLenth,Info.ChunkSize,DataQueue,OnProcessFile,OnFileCompelete);
	FRunnableThread::Create(FileWorker,TEXT("FileWorker"));
	FHTTPDownloadWorker* DownloadWorker = new FHTTPDownloadWorker(Info.Url,Info.FileLenth,Info.ChunkSize,DataQueue,this->OnDownloadCompelete,this->OnProcessDownload);
	FRunnableThread::Create(DownloadWorker,TEXT("DownloadWorker"));
}

FHTTPDownloadWorker::FHTTPDownloadWorker(FString Url, int32 FileLenth,int32 ChunkSize,
                                        TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue, const FOnDownloadCompelete& DownloadCompeleteDelegate,
                                         const FOnProcessDownload& OnProcessDownload)
{
	this->Url=Url;
	this->ChunkSize = ChunkSize;
	this->DownloadCompeleteDelegate = DownloadCompeleteDelegate;
	this->OnProcessDownload = OnProcessDownload;
	this->FileLenth = FileLenth;
	this->DataQueue = DataQueue;

}

FHTTPDownloadWorker::~FHTTPDownloadWorker()
{
}

bool FHTTPDownloadWorker::Init()
{
	return FRunnable::Init();
}

uint32 FHTTPDownloadWorker::Run()
{
	while (!bDownloadComplete)
	{
		//只分配一次对象
		if (DoOnce == false)
		{
			FOneChunkInfo ChunkInfo;
			for (int Count = 0; Count <MAX_PROCESS; ++Count)
			{
				ApplyForDownload(ChunkInfo);
				TUniquePtr<DownloadObject> Object = MakeUnique<DownloadObject>(Count,Url,ChunkSize,DataQueue);
				Object.Get()->StartDownload.ExecuteIfBound(ChunkInfo);
				ObjectPool[Count] = MoveTemp(Object);
			}
			DoOnce = true;
		}
	}
	return 0;
}

void FHTTPDownloadWorker::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("Download Thread Exit!"));
	FRunnable::Stop();
}

void FHTTPDownloadWorker::Exit()
{
	FRunnable::Exit();
	UE_LOG(LogTemp, Warning, TEXT("Download Thread Exit!"));
}

void FHTTPDownloadWorker::ApplyForDownload(FOneChunkInfo& ChunkInfo)
{
	int32 NextLenth = FMath::Min(ChunkSize,FileLenth-PositionNow);
	if (NextLenth < ChunkSize)
	{
		ChunkInfo.StartPosition = PositionNow;
		ChunkInfo.bFinalContent = true;
		PositionNow = PositionNow+NextLenth;
		ChunkInfo.Lenth = NextLenth;
	}
	else
	{
		ChunkInfo.StartPosition = PositionNow;
		ChunkInfo.Lenth = NextLenth;
		ChunkInfo.bFinalContent = false;
		PositionNow = PositionNow+ChunkSize;
	}
}

void FHTTPDownloadWorker::AssignWork(int32 Id, EHttpStatus Status,const FOneChunkInfo& ChunkInfo)
{
	if (Status == EHttpStatus::OK)
	{
		FOneChunkInfo ChunkInfoLocal;
		ApplyForDownload(ChunkInfoLocal);
		ObjectPool[Id].Get()->StartDownload.ExecuteIfBound(ChunkInfoLocal);
	}
	else if(Status==EHttpStatus::OK && bDownloadComplete)
	{
		
	}
	else
	{
		ChunkInfoQueue.Enqueue(ChunkInfo);
	}
}


FHTTPFileWorker::FHTTPFileWorker(FString FilePath,int32 FileLenth,int32 ChunkSize,TSharedPtr<TQueue<FDownloadData,EQueueMode::Spsc>> DataQueue
                                 ,const FOnProcessFile& OnProcessFile,const FOnFileCompelete& OnFileCompelete)
{

	this->FilePath = FilePath;
	this->FileLenth = FileLenth;
	this->ChunkSize = ChunkSize;
	this->OnProcessFile = OnProcessFile;
	this->OnFileCompelete = OnFileCompelete;
	this->DataQueue = DataQueue;
}

FHTTPFileWorker::~FHTTPFileWorker()
{
}

bool FHTTPFileWorker::Init()
{
	UDownloadSubsystem::OnEnqueueData.BindRaw(this,&FHTTPFileWorker::AddQueueDataNum);
	UDownloadSubsystem::DownloadCompelete.BindRaw(this,&FHTTPFileWorker::ChangeDownloadCompeleteStatus);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	OutputFileHandle = PlatformFile.OpenWrite(*FilePath);
	FString MD5File = FilePath+".md5";
	MD5FileHandle = PlatformFile.OpenWrite(*MD5File,false);
	return true;
}

uint32 FHTTPFileWorker::Run()
{
	while (!bDownloadCompelete||QueueDataNum.GetValue()!=0)
	{
		if (QueueDataNum.GetValue()!=0&&DataQueue)
		{
			FDownloadData Data;
			if (DataQueue->Dequeue(Data))
			{
				QueueDataNum.Decrement();
				UpdatePercent(Data);
				if (OutputFileHandle)
				{
					if (OutputFileHandle->Seek(Data.StartPosition))
					{
						UE_LOG(LogTemp, Warning, TEXT("Seek to %d Compelete"),Data.StartPosition);
						if(OutputFileHandle->Write(Data.Data.GetData(),Data.Data.Num()))
						{
							UE_LOG(LogTemp, Warning, TEXT("Write to %d Compelete"),Data.StartPosition+Data.Data.Num());
						}
					}
				}
			}
		}
	}
	delete OutputFileHandle;
	delete MD5FileHandle;
	return 0;
}

void FHTTPFileWorker::Stop()
{
	bDownloadCompelete = true;
	QueueDataNum.Reset();
	FRunnable::Stop();
}

void FHTTPFileWorker::Exit()
{
	bDownloadCompelete = true;
	QueueDataNum.Reset();
	FRunnable::Exit();
}

void FHTTPFileWorker::UpdatePercent(const FDownloadData& Data)
{
	MaxPosition = FMath::Max(Data.StartPosition,MaxPosition);
	float Percent =FMath::GetRangePct(float(0),float(FileLenth),float(MaxPosition+Data.Data.Num()));
	this->OnProcessFile.ExecuteIfBound(Percent);
}

void FHTTPFileWorker::AddQueueDataNum()
{
	QueueDataNum.Increment();
}

void FHTTPFileWorker::ChangeDownloadCompeleteStatus()
{
	bDownloadCompelete = true;
}

FHttpFileAllocator::FHttpFileAllocator(FString FilePath, uint64 FileSize, int32 ChunkSize,
                                       const FOnFilePreAllocCompelete& PreAllocDelegate)
{
	this->FilePath = FilePath;
	this->FileSize = FileSize;
	this->ChunkSize = ChunkSize;
	this->PreAllocDelegate = PreAllocDelegate;
}

FHttpFileAllocator::~FHttpFileAllocator()
{
}

bool FHttpFileAllocator::Init()
{
	return FRunnable::Init();
}

uint32 FHttpFileAllocator::Run()
{
	FString MD5File = FilePath+".md5";
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath,false);
	IFileHandle* MD5Handle = PlatformFile.OpenWrite(*MD5File,false);
	if (FileHandle == nullptr || MD5Handle == nullptr)
	{
		return 0;
	}
	// 分配缓冲区
	const uint32 BufferSize = 1024 * 1024;
	const uint32 MD5ChunkSize = 16;
	uint8* Buffer = new uint8[BufferSize];
	uint8* MD5Buffer = new uint8[MD5ChunkSize];
	FMemory::Memset(Buffer, 0, BufferSize);
	FMemory::Memset(MD5Buffer,0,MD5ChunkSize);

	// 写入数据块
	const uint64 NumBlocks = FileSize / BufferSize;
	const uint32 Remainder = FileSize % BufferSize;
	const int32 ChunkNum = FileSize/ChunkSize+1;
	for (int32 i = 0;i<ChunkNum;i++)
	{
		if (bIsStopping)
		{
			break;
		}
		if (!MD5Handle->Write(MD5Buffer,MD5ChunkSize))
		{
			break;
		}
	}
	for (uint64 i = 0; i < NumBlocks; i++)
	{
		if (bIsStopping)
		{
			break;
		}
		
		if (!FileHandle->Write(Buffer, BufferSize))
		{
			break;
		}

		NumBlocksWritten.Increment();
	}

	// 写入剩余数据
	if (Remainder > 0 && !bIsStopping)
	{
		const uint32 BytesWritten = FileHandle->Write(Buffer, Remainder);
		if (BytesWritten == Remainder)
		{
			NumBlocksWritten.Increment();
		}
	}

	

	// 关闭文件
	delete[] Buffer;
	delete[] MD5Buffer;
	delete MD5Handle;
	delete FileHandle;
	bIsFinished = true;
	return 0;
}

void FHttpFileAllocator::Stop()
{
	FRunnable::Stop();
	bIsStopping = true;
}

void FHttpFileAllocator::Exit()
{
	FRunnable::Exit();
	UE_LOG(LogTemp, Warning, TEXT("PreAlloc EXIT!"));
	PreAllocDelegate.ExecuteIfBound();
}
