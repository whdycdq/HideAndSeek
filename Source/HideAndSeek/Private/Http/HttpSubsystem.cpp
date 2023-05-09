// Fill out your copyright notice in the Description page of Project Settings.


#include "Http/HttpSubsystem.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Serialization/BufferArchive.h"
int32 FDownloadWorker::PositionNow = 0;
int32 FDownloadWorker::ChunkSize = 0;
int32 FDownloadWorker::FileLenth = 0;
FCriticalSection FDownloadWorker::Mutex;
FCriticalSection FDownloadWorker::ExitMutex;
FCriticalSection FDownloadWorker::QueueCounterMutex;
FThreadSafeCounter FDownloadWorker::NumThreads;
FThreadSafeCounter FDownloadWorker::ChunkCounter;

FFilePreAllocator::~FFilePreAllocator()
{
}

bool FFilePreAllocator::Init()
{
	return FRunnable::Init();
}

uint32 FFilePreAllocator::Run()
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

void FFilePreAllocator::Stop()
{
	FRunnable::Stop();
	bIsStopping = true;
}

void FFilePreAllocator::Exit()
{
	FRunnable::Exit();
	UE_LOG(LogTemp, Warning, TEXT("PreAlloc EXIT!"));
	PreAllocDelegate.ExecuteIfBound();
}


UHttpSubsystem::UHttpSubsystem()
{
	Bus = UHASDelegateBus::GetInstance();
}

void UHttpSubsystem::UserLogin(FString Username, FString Password)
{
	
	// 对字符串进行SHA-256散列处理
	FSHAHash HashedPassword;
	// 将散列处理后的结果转换为十六进制字符串
	FString HashedPasswordString = HashedPassword.ToString();
	// 创建HTTP请求对象
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 设置请求URL
	Request->SetURL(ServerBaseUrl+"login");

	// 设置请求方法为POST
	Request->SetVerb(TEXT("POST"));

	// 添加POST参数
	FString PostData = FString::Printf(TEXT("username=%s&password=%s"), *Username, *HashedPasswordString);
	Request->SetContentAsString(PostData);

	// 设置请求头
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	// 发送请求
	Request->OnProcessRequestComplete().BindUObject(this,&UHttpSubsystem::OnLoginUserResponseReceived);
	Request->ProcessRequest();
}

void UHttpSubsystem::UserRegister(FString Username, FString Password)
{
	// 对字符串进行SHA-256散列处理
	FSHAHash HashedPassword;
	FSHA1::HashBuffer(TCHAR_TO_UTF8(*Password), Password.Len(), HashedPassword.Hash);
	// 将散列处理后的结果转换为十六进制字符串
	FString HashedPasswordString = HashedPassword.ToString();

	// 创建HTTP请求对象
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 设置请求URL
	Request->SetURL(ServerBaseUrl+"register");

	// 设置请求方法为POST
	Request->SetVerb(TEXT("POST"));

	// 添加POST参数
	FString PostData = FString::Printf(TEXT("username=%s&password=%s"), *Username, *HashedPasswordString);
	Request->SetContentAsString(PostData);

	// 设置请求头
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	// 发送请求
	Request->OnProcessRequestComplete().BindUObject(this,&UHttpSubsystem::OnRegisterUserResponseReceived);
	Request->ProcessRequest();
}

void UHttpSubsystem::UserUpdatePassword(FString Username, FString OldPassword, FString NewPassword)
{
	// 对字符串进行SHA-256散列处理
	FSHAHash HashedPassword;
	HashedPassword.FromString(OldPassword);
	// 将散列处理后的结果转换为十六进制字符串
	FString HashedOldPasswordString = HashedPassword.ToString();
	HashedPassword.FromString(NewPassword);
	FString HashedNewPasswordString = HashedPassword.ToString();
	// 创建HTTP请求对象
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 设置请求URL
	Request->SetURL(ServerBaseUrl+"register");

	// 设置请求方法为POST
	Request->SetVerb(TEXT("POST"));

	// 添加POST参数
	FString PostData = FString::Printf(TEXT("username=%s&password=%s&newpassword=%s"), *Username, *HashedOldPasswordString,*HashedNewPasswordString);
	Request->SetContentAsString(PostData);

	// 设置请求头
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));

	// 发送请求
	Request->OnProcessRequestComplete().BindUObject(this,&UHttpSubsystem::OnPasswordUserResponseReceived);
	Request->ProcessRequest();
}

void UHttpSubsystem::StartDownloadThread()
{
	FOnEnqueueData OnEnqueueData;
	FDownloadWorker* WorkerInit = new FDownloadWorker(this->DownloadInfo.Url,this->DownloadInfo.FilePath,this->DownloadInfo.ThreadNum,DataQueue,DownloadCompeleteDelegate,OnProcessDownload,DownloadCompelete,OnEnqueueData);
	WorkerInit->SetChunkSize(this->DownloadInfo.ChunkSize);
	WorkerInit->SetFileLenth(this->DownloadInfo.FileLenth);
	WorkerInit->SetPosition(0);


	FFileWorker* FileWorker = new FFileWorker(this->DownloadInfo.FilePath,this->DownloadInfo.FileLenth,this->DownloadInfo.ChunkSize,DataQueue,DownloadCompeleteDelegate,OnProcessFile,OnFileCompelete,OnEnqueueData);
	RunningDownloadThreads.Add(FRunnableThread::Create(FileWorker, TEXT("FileWorker")));
	RunningDownloadThreads.Add(FRunnableThread::Create(WorkerInit, TEXT("DownloadWorker")));
	for (int Counter = 0;Counter< this->DownloadInfo.ThreadNum-1;++Counter)
	{
		FDownloadWorker* Worker = new FDownloadWorker(this->DownloadInfo.Url,this->DownloadInfo.FilePath,this->DownloadInfo.ThreadNum,DataQueue,DownloadCompeleteDelegate,OnProcessDownload,DownloadCompelete,OnEnqueueData);
		RunningDownloadThreads.Add(FRunnableThread::Create(Worker, TEXT("DownloadWorker")));
	}
}

void UHttpSubsystem::StartDownload(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	
	if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetContentLength() <= 0)
	{
		return;
	}
	
	else
	{
		this->DownloadInfo.FileLenth = HttpResponse->GetContentLength();
		PreAllocFile(this->DownloadInfo.FilePath,this->DownloadInfo.FileLenth,this->DownloadInfo.ChunkSize);
	}
}

void UHttpSubsystem::DownloadSync(FString Url,FString FilePath,int32 ChunkSize,int32 ThreadNum,const FOnProcessDownload& OnProcessDownloadDelegate,const FDownloadCompelete& OnDownloadCompeleteDelegate,const FOnProcessFile& OnProcessFileDelegate,const FOnFileCompelete& OnFileCompeleteDelegate)
{
	this->OnProcessDownload = OnProcessDownloadDelegate;
	this->DownloadCompelete = OnDownloadCompeleteDelegate;
	this->OnProcessFile = OnProcessFileDelegate;
	this->OnFileCompelete = OnFileCompeleteDelegate;
	this->DownloadInfo.Url=Url;
	this->DownloadInfo.FilePath = FilePath;
	this->DownloadInfo.ChunkSize = ChunkSize;
	this->DownloadInfo.ThreadNum = ThreadNum;
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
	Request->SetVerb("HEAD");
	Request->SetURL(Url);
	Request->OnProcessRequestComplete().BindUObject(this,&UHttpSubsystem::StartDownload);
	Request->ProcessRequest();
}

void UHttpSubsystem::DownloadMultiFile(TArray<FDownloadFileRequire>& FileArray, int32 ChunkSize, int32 ThreadNum,
	const FOnProcessDownload& OnProcessDownloadDelegate, const FDownloadCompelete& OnDownloadCompeleteDelegate,
	const FOnProcessFile& OnProcessFileDelegate, const FOnFileCompelete& OnFileCompeleteDelegate)
{
	
}


void UHttpSubsystem::SetDownloadPath(FString Path)
{
	SavePath = Path;
}

void UHttpSubsystem::SetDownloadUrl(FString Url)
{
	URL = Url;
}

void UHttpSubsystem::ProcessOnRequestDownload(int32& Position, bool& bFinalContent, int32& Lenth)
{
	if ((DownloadInfo.FileLenth - this->PositionNow)>0)
	{
		int32 NewLen= FMath::Min(DownloadInfo.ChunkSize,DownloadInfo.FileLenth - this->PositionNow);
		if (NewLen <= this->DownloadInfo.ChunkSize)
		{
			Position= PositionNow;
			bFinalContent = true;
			Lenth = NewLen;
			PositionNow = Position + NewLen;
		}
		Position = PositionNow;
		bFinalContent = false;
		PositionNow = PositionNow + this->DownloadInfo.ChunkSize;
		Lenth = this->DownloadInfo.ChunkSize;
	}
	Position = PositionNow;
	bFinalContent = true;
	Lenth = 0;
}

bool UHttpSubsystem::UnmarshalVersionJson(const FString& Json,TArray<FDownloadFileRequire>&FileArray)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<TCHAR>>JsonReader = TJsonReaderFactory<TCHAR>::Create(Json);
	if (FJsonSerializer::Deserialize(JsonReader,JsonObject))
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField("File");
		for (auto Value:JsonArray)
		{
			FDownloadFileRequire DownloadFileRequire;
			TSharedPtr<FJsonObject> JsonObj = Value->AsObject();
			DownloadFileRequire.Version = JsonObj->GetStringField("Version");
			DownloadFileRequire.Filename = JsonObj->GetStringField("Filename");
			DownloadFileRequire.Url = JsonObject->GetStringField("Url");
			DownloadFileRequire.MD5 = JsonObject->GetStringField("MD5");
			FileArray.Add(DownloadFileRequire);
		}
		return true;
	}
	return false;
}

void UHttpSubsystem::PreAllocFile(const FString& FilePath, int32 FileSize,int32 ChunkSize)
{
	PreAllocDelegate.BindLambda([this](){StartDownloadThread();});
	FFilePreAllocator* Allocor = new FFilePreAllocator(FilePath,FileSize,ChunkSize,PreAllocDelegate);
	FRunnableThread::Create(Allocor,TEXT("AllocWorker"));
}
void UHttpSubsystem::OnPreAllocCompelete()
{
	UE_LOG(LogTemp, Warning, TEXT("PreAllocCompelete!"));
	StartDownloadThread();
}

void UHttpSubsystem::HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid()) {
		return;
	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) {
		//保存文件
		FString Filename = FPaths::GetCleanFilename(HttpRequest->GetURL());
		FString Left;
		FString Right;
		Filename.Split("?", &Left, &Right);
		FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *FString::Printf(TEXT("%s%s"), *FPaths::ProjectContentDir(), *Left));
	}
}

void UHttpSubsystem::HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	
}


void UHttpSubsystem::OnRegisterUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (bSuccess && Response.IsValid())
	{
		// 解析服务器响应的JSON数据
		FString ResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// 从JSON数据中获取响应消息
			FString Message;
			if (JsonObject->TryGetStringField(TEXT("message"), Message))
			{
				UE_LOG(LogTemp, Warning, TEXT("Register user success: %s"), *Message);
			}
			else if (JsonObject->TryGetStringField(TEXT("error"), Message))
			{
				UE_LOG(LogTemp, Warning, TEXT("Register user failed: %s"), *Message);
			}
		}
	}
}

void UHttpSubsystem::OnPasswordUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (bSuccess && Response.IsValid())
	{
		// 解析服务器响应的JSON数据
		FString ResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// 从JSON数据中获取响应消息
			FString Message;
			if (JsonObject->TryGetStringField(TEXT("message"), Message))
			{
				UE_LOG(LogTemp, Warning, TEXT("Register user success: %s"), *Message);
			}
			else if (JsonObject->TryGetStringField(TEXT("error"), Message))
			{
				UE_LOG(LogTemp, Warning, TEXT("Register user failed: %s"), *Message);
			}
		}
	}
}

FFileWorker::FFileWorker(FString FileDir, int32 FileLenth, int32 ChunkSize,
	TQueue<FDownloadChunkInfo, EQueueMode::Mpsc>& DataQueue, const FOnDownloadCompelete& DownloadCompeleteDelegate,
	const FOnProcessFile& OnProcessFile, const FOnFileCompelete& OnFileCompelete,const FOnEnqueueData& OnEnqueueData):
	FileDir(FileDir)
	,FileLenth(FileLenth)
	,ChunkSize(ChunkSize)
	,DataQueue(DataQueue)
	,DownloadCompeleteDelegate(DownloadCompeleteDelegate)
	,OnProcessFile(OnProcessFile)
	,OnFileCompelete(OnFileCompelete)
	,OnEnqueueData(OnEnqueueData)
{
}

FFileWorker::~FFileWorker()
{
	
}

bool FFileWorker::Init()
{
	DownloadCompeleteDelegate.BindLambda([=](){this->bDownloadCompelete=true;});
	OnEnqueueData.BindLambda([=](){UE_LOG(LogTemp, Warning, TEXT("hello world"));});
	int32 ChunkNum = FileLenth/ChunkSize +1;
	MD5Array = new uint8[ChunkNum*16];
	//this->DownloadCompeleteDelegate.BindLambda([this](){UE_LOG(LogTemp, Warning, TEXT("DownloadCompelete"));this->bDownloadCompelete = true;});
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	OutputFileHandle = PlatformFile.OpenWrite(*FileDir);
	FString MD5File = FileDir+".md5";
	MD5FileHandle = PlatformFile.OpenWrite(*MD5File,false);
	
	/*
	const uint8 Zero = 0;
	if (OutputFileHandle)
	{
		OutputFileHandle->Seek(FileLenth);
		bool bSuccess =  OutputFileHandle->Write(&Zero,FileLenth);
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("File PreDownload Compelete"));
			return true;
		}
		this->Stop();
	}
	return false;
	*/
	return true;
}

uint32 FFileWorker::Run()
{
	while (!bDownloadCompelete)
	{
		if (QueueCounter!=0)
		{
			FDownloadChunkInfo Info;
			if (DataQueue.Dequeue(Info))
			{
				--QueueCounter;
				if (OutputFileHandle)
				{
					if (OutputFileHandle->Seek(Info.StartPosition))
					{
						UE_LOG(LogTemp, Warning, TEXT("Seek to %d Compelete"),Info.StartPosition);
						if(OutputFileHandle->Write(Info.Data.GetData(),Info.Data.Num())&&WriteMD5(Info))
						{
							UpdatePercent(Info);
						}
					}
				}
			}
		}
	}
	delete OutputFileHandle;
	delete MD5FileHandle;
	FMD5Hash hash = FMD5Hash::HashFile(*FileDir);
	return 0;
}

void FFileWorker::Stop()
{
	FRunnable::Stop();
	bDownloadCompelete = true;
}

void FFileWorker::Exit()
{
	bDownloadCompelete = true;
}

void FFileWorker::OnDownloadComplete()
{
	bDownloadCompelete = true;
}

bool FFileWorker::WriteMD5(FDownloadChunkInfo& Info)
{
	//Md5Gen.Update(Info.Data.GetData(),Info.Data.Num());
	/*
	FString out = FMD5::HashBytes(Info.Data.GetData(),Info.Data.Num());
	FMemory::Memcpy(MD5Array+(Info.ChunkNum*16),Digest,16);
	if(MD5FileHandle->Seek(Info.ChunkNum*16))
	{
		if(MD5FileHandle->Write(Digest,16))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"),*out);
			return true;
		}
		return false;
	}
	 */
	/*Hasher.Update(Info.Data.GetData(),Info.Data.Num());
	Hasher.Final(FinalMD5.GetData());
	if (!FinalMD5.Num()!=0)
	{
		if (MD5FileHandle)
		{
			if(MD5FileHandle->Seek(Info.ChunkNum*16))
			{
				if(MD5FileHandle->Write(FinalMD5.GetData(),FinalMD5.Num()))
				{
					return true;
				}
				return false;
			}
			return false;
		}
		return false;
	}
	return false;
	*/
	return true;
}

void FFileWorker::UpdatePercent(FDownloadChunkInfo& Info)
{
	MaxPosition = FMath::Max(Info.StartPosition,MaxPosition);
	float Percent =FMath::GetRangePct(float(0),float(FileLenth),float(MaxPosition+Info.Data.Num()));
	this->OnProcessFile.ExecuteIfBound(Percent);
}


void UHttpSubsystem::OnLoginUserResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (bSuccess && Response.IsValid())
	{
		// 解析服务器响应的JSON数据
		FString ResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		FBroadCast BroadCast;
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// 从JSON数据中获取响应消息
			FString UID;
			FString Token;
			FString Username;
			if (JsonObject->TryGetStringField(TEXT("uid"), UID)
				&& JsonObject->TryGetStringField(TEXT("username"),Username)
				&& JsonObject->TryGetStringField(TEXT("token"),Token))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Register user success:\n pid:%s \n username:%s \n token:%s \n"), *UID,*Username,*Token);
				BroadCast.Info.Token=Token;
				BroadCast.Info.PlayerName=Username;
				BroadCast.Info.UserID=UKismetStringLibrary::Conv_StringToInt(UID);
				BroadCast.Info.CharacterType = 1;
				if (Bus)
				{
					Bus->LoginResponseDelegate.ExecuteIfBound(BroadCast);
				}
			}
			else if (JsonObject->TryGetStringField(TEXT("error"), UID))
			{
				UE_LOG(LogTemp, Warning, TEXT("Register user failed: %s"), *UID);
			}
		}
	}
}


FDownloadWorker::~FDownloadWorker()
{
}

bool FDownloadWorker::Init()
{
	NumThreads.Increment();
	return FRunnable::Init();
}

uint32 FDownloadWorker::Run()
{
	bool bFinalFile;
	int32 Lenth;
	ProcessOnRequestDownload(this->StartPosition,this->ChunkNow,bFinalFile,Lenth);
	ProcessDownload(bFinalFile,ChunkSize);
	bStartedDownload = true;
	while (!bExit)
	{
		
	}
	return 0;
}

void FDownloadWorker::Stop()
{
	NumThreads.Decrement();
	FRunnable::Stop();
	DownloadCompeleteDelegate.ExecuteIfBound();
}

void FDownloadWorker::Exit()
{
	FRunnable::Exit();
	NumThreads.Decrement();
	if (NumThreads.GetValue()==0)
	{
		ExitMutex.Lock();
		UE_LOG(LogTemp, Warning, TEXT("Thread EXIT" ));
		PositionNow = 0;
		ChunkCounter.Reset();
		if(DownloadCompeleteDelegate.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Warning, TEXT("DownloadCompeleteDelegate执行" ));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DownloadCompeleteDelegate未执行" ));
		}
		ExitMutex.Unlock();
	}
}

void FDownloadWorker::ProcessDownload(bool bFinalContent,int32 Lenth)
{
	if (!bFinalContent)
	{
		const int32 EndPos = StartPosition+ChunkSize;
		FString RangeHeader = FString::Printf(TEXT("bytes=%d-%d"), StartPosition, EndPos - 1);
		UE_LOG(LogTemp, Warning, TEXT("Thread Download Position : %d  to  %d"), StartPosition,EndPos);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb(TEXT("GET"));
		HttpRequest->SetURL(Url);
		HttpRequest->SetHeader(TEXT("Range"), RangeHeader);
		float Percent =FMath::GetRangePct(float(0),float(FileLenth),float(EndPos));
		OnProcessDownload.ExecuteIfBound(Percent);
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &FDownloadWorker::OnRequestComplete);
		HttpRequest->ProcessRequest();
	}
	else if (bFinalContent&&Lenth!=0)
	{
		FString RangeHeader = FString::Printf(TEXT("bytes=%d-%d"), StartPosition, StartPosition+Lenth-1);
		UE_LOG(LogTemp, Warning, TEXT("Thread Download Position : %d  to  %d"), StartPosition,StartPosition+Lenth-1);
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetVerb(TEXT("GET"));
		HttpRequest->SetURL(Url);
		HttpRequest->SetHeader(TEXT("Range"), RangeHeader);
		float Percent =FMath::GetRangePct(float(0),float(FileLenth),float(StartPosition+Lenth));
		OnProcessDownload.ExecuteIfBound(Percent);
		HttpRequest->OnProcessRequestComplete().BindRaw(this, &FDownloadWorker::OnRequestComplete);
		HttpRequest->ProcessRequest();
	}
	else
	{
		bExit = true;
	}
}


void FDownloadWorker::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid() || Response->GetResponseCode() != 206)
	{
		
	}
	else
	{
		FDownloadChunkInfo Info;
		Info.Data=Response->GetContent();
		Info.StartPosition = this->StartPosition;
		Info.ChunkNum = ChunkNow;
		if (DataQueue.Enqueue(Info))
		{
			QueueCounterMutex.Lock();
			if (this->OnEnqueueData.ExecuteIfBound())
			{
				UE_LOG(LogTemp, Warning, TEXT("EnqueueDataDelegate执行%s"),*this->OnEnqueueData.TryGetBoundFunctionName().ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EnqueueDataDelegate未执行%s" ),*this->OnEnqueueData.TryGetBoundFunctionName().ToString());
			}
			QueueCounterMutex.Unlock();
		}
		bool bFinalContent;
		int32 Lenth;
		ProcessOnRequestDownload(this->StartPosition,ChunkNow,bFinalContent,Lenth);
		ProcessDownload(bFinalContent,Lenth);
	}
}

void FDownloadWorker::ProcessOnRequestDownload(int32& Position,int32& ChunkNum, bool& bFinalContent, int32& Lenth)
{
	Mutex.Lock();
	if ((FileLenth - PositionNow)>0)
	{
		int32 NewLen= FMath::Min(ChunkSize,FileLenth - PositionNow);
		if (NewLen < ChunkSize)
		{
			
			Position= PositionNow;
			bFinalContent = true;
			Lenth = NewLen;
			PositionNow = Position + NewLen;
			ChunkNum = ChunkCounter.GetValue();
			ChunkCounter.Increment();
			Mutex.Unlock();
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Get Download Position : %d"), PositionNow);
		Position = PositionNow;
		bFinalContent = false;
		PositionNow = PositionNow + ChunkSize;
		Lenth = ChunkSize;
		ChunkNum = ChunkCounter.GetValue();
		ChunkCounter.Increment();
		Mutex.Unlock();
		return;
	}
	Position = PositionNow;
	bFinalContent = true;
	Lenth = 0;
	ChunkNum = 0;
	Mutex.Unlock();
}

void FDownloadWorker::AddExitNum()
{
	ExitMutex.Lock();
	ExitMutex.Unlock();
}

void FDownloadWorker::SetFileLenth(int32 initFileLenth)
{
	FileLenth = initFileLenth;
}
void FDownloadWorker::SetChunkSize(int32 initChunkSize)
{
	ChunkSize = initChunkSize;
}

void FDownloadWorker::SetPosition(int initSetPosition)
{
	PositionNow = initSetPosition;
}



