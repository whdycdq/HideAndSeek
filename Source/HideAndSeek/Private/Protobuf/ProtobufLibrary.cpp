// Fill out your copyright notice in the Description page of Project Settings.


#include "Protobuf/ProtobufLibrary.h"


UProtobufLibrary::UProtobufLibrary()
{
	
}

bool UProtobufLibrary::MarshalMovementMessage(MovementStruct* Movement,const FPlayerMovementInfo& Info, TArray<uint8>& Output)
    {
	TSharedPtr<MovementStruct> MovementStruct = MakeShareable(new struct MovementStruct);
		     				if (MovementStruct->NewInfo->IsInitialized()
		     					&&MovementStruct->NewLocation->IsInitialized()
		     					&&MovementStruct->NewRotator->IsInitialized()
		     					&&MovementStruct->NewCRotator->IsInitialized()
		     					&&MovementStruct->NewTrans->IsInitialized())
		     				{
		     					UE_LOG(LogTemp, Log, TEXT("执行到判PROTOBUF判空"));
		     					MovementStruct->NewLocation->set_x(Info.Transform.GetLocation().X);
		     					MovementStruct->NewLocation->set_y(Info.Transform.GetLocation().Y);
		     					MovementStruct->NewLocation->set_z(Info.Transform.GetLocation().Z);
		     					MovementStruct->NewRotator->set_pitch(Info.Transform.Rotator().Pitch);
		     					MovementStruct->NewRotator->set_roll(Info.Transform.Rotator().Roll);
		     					MovementStruct->NewRotator->set_yaw(Info.Transform.Rotator().Yaw);
		     					MovementStruct->NewScale->set_x(Info.Transform.GetScale3D().X);
		     					MovementStruct->NewScale->set_y(Info.Transform.GetScale3D().Y);
		     					MovementStruct->NewScale->set_z(Info.Transform.GetScale3D().Z);
		     					MovementStruct->NewCRotator->set_pitch(Info.ControllerRotation.Pitch);
		     					MovementStruct->NewCRotator->set_yaw(Info.ControllerRotation.Yaw);
		     					MovementStruct->NewCRotator->set_roll(Info.ControllerRotation.Roll);
		     					MovementStruct->Axis->set_moveforward(Info.Axis.MoveForward);
		     					MovementStruct->Axis->set_moveright(Info.Axis.MoveRight);
		     					MovementStruct->NewTrans->set_allocated_l(MovementStruct->NewLocation);
		     					MovementStruct->NewTrans->set_allocated_r(MovementStruct->NewRotator);
		     					MovementStruct->NewTrans->set_allocated_s(MovementStruct->NewScale);
		     					MovementStruct->NewInfo->set_allocated_crotation(MovementStruct->NewCRotator);
		     					MovementStruct->NewInfo->set_allocated_trans(MovementStruct->NewTrans);
		     					MovementStruct->NewInfo->set_allocated_inputaxis(MovementStruct->Axis);
		     					Output.SetNum(MovementStruct->NewInfo->ByteSizeLong());
		     					if (MovementStruct->NewInfo->SerializeToArray(Output.GetData(),Output.Num()))
		     					{
		     						UE_LOG(LogTemp, Log, TEXT("执行到判PROTOBUF序列化"));
		     						return true;
		     					}
		     					return false;
                                                        		
		     				}
		     			return false;
	   
    }


	bool UProtobufLibrary::UnmarshalMovementMessage(TArray<uint8> Input,FPlayerMovementInfo& Info)
    {
	TSharedPtr<MovementStruct> MovementStruct = MakeShareable(new struct MovementStruct);
		    if (MovementStruct->NewInfo->ParseFromArray(Input.GetData(),Input.Num()))
		    {
		    	Info.Axis.MoveForward=MovementStruct->NewInfo->inputaxis().moveforward();
		    	Info.Axis.MoveRight = MovementStruct->NewInfo->inputaxis().moveright();
			    Info.Transform.SetLocation(FVector(MovementStruct->NewInfo->trans().l().x(),MovementStruct->NewInfo->trans().l().y(),MovementStruct->NewInfo->trans().l().z()));
		    	Info.Transform.SetRotation(FRotator(MovementStruct->NewInfo->trans().r().pitch(),MovementStruct->NewInfo->trans().r().yaw(),MovementStruct->NewInfo->trans().r().roll()).Quaternion());
		    	Info.Transform.SetScale3D(FVector(MovementStruct->NewInfo->trans().s().x(),MovementStruct->NewInfo->trans().s().y(),MovementStruct->NewInfo->trans().s().z()));
		    	Info.ControllerRotation.Pitch = MovementStruct->NewInfo->crotation().pitch();
		    	Info.ControllerRotation.Roll = MovementStruct->NewInfo->crotation().roll();
		    	Info.ControllerRotation.Yaw = MovementStruct->NewInfo->crotation().yaw();
		    	Input.Empty();
		    	return true;
		    }
	    	return false;
		
    }

bool UProtobufLibrary::UnmarshalToken(const TArray<uint8>& Input, FPlayerInfo& Info)
{
	Player::SyncPlayerInfo* TempInfo =new Player::SyncPlayerInfo;
	if (TempInfo!=nullptr)
	{
		if(TempInfo->ParseFromArray(Input.GetData(),Input.Num()))
		{
			Info.Token=TempInfo->token().token().c_str();
			Info.PlayerName=TempInfo->name().c_str();
			Info.UserID=TempInfo->pid();
			Info.CharacterType = TempInfo->charactertype();
			return true;
		}
		return false;
	}
	return false;
}

bool UProtobufLibrary::UnmarshalBroadCast(const TArray<uint8>& Input, FBroadCast& Info)
{
	TSharedPtr <BroadcastStruct> BroadCastInfo = MakeShareable(new struct BroadcastStruct) ;
	if (BroadCastInfo!=nullptr)
	{
		if (BroadCastInfo->Temp->ParseFromArray(Input.GetData(),Input.Num()))
		{
			Info.Info.Token=BroadCastInfo->Temp->token().token().token().c_str();
			Info.Info.PlayerName=BroadCastInfo->Temp->token().name().c_str();
			Info.Info.UserID=BroadCastInfo->Temp->token().pid();
			Info.Info.CharacterType=BroadCastInfo->Temp->token().charactertype();
			Info.Content = BroadCastInfo->Temp->content();
			Info.ActionData = BroadCastInfo->Temp->actiondata();
			Info.Movement.Axis.MoveForward = BroadCastInfo->Temp->info().inputaxis().moveforward();
			Info.Movement.Axis.MoveRight = BroadCastInfo->Temp->info().inputaxis().moveright();
			Info.Movement.Transform.SetLocation(FVector(BroadCastInfo->Temp->info().trans().l().x(),BroadCastInfo->Temp->info().trans().l().y(),BroadCastInfo->Temp->info().trans().l().z()));
			Info.Movement.Transform.SetRotation(FRotator(BroadCastInfo->Temp->info().trans().r().pitch(),BroadCastInfo->Temp->info().trans().r().yaw(),BroadCastInfo->Temp->info().trans().r().roll()).Quaternion());
			Info.Movement.Transform.SetScale3D(FVector(BroadCastInfo->Temp->info().trans().s().x(),BroadCastInfo->Temp->info().trans().s().y(),BroadCastInfo->Temp->info().trans().s().z()));
			Info.Movement.ControllerRotation.Pitch = BroadCastInfo->Temp->info().crotation().pitch();
			Info.Movement.ControllerRotation.Roll = BroadCastInfo->Temp->info().crotation().roll();
			Info.Movement.ControllerRotation.Yaw = BroadCastInfo->Temp->info().crotation().yaw();
			return true;
		}
		return false;
	}
	return false;
}
/*bool UProtobufLibrary::MarshalTokenByLocalController(TArray<uint8>& Output)
  {
  	Player::SyncPlayerInfo PlayerInfo;
  	Player::SyncPlayerToken Token;
  	AHASPlayerControllerBase* Controller = Cast<AHASPlayerControllerBase>(UGameplayStatics::GetPlayerController(this,0));
  	if (Controller)
  	{
  		Token.set_token(TCHAR_TO_UTF8(*Controller->Info.Token));
  		PlayerInfo.set_allocated_token(&Token);
  		PlayerInfo.set_pid(Controller->Info.UserID);
  		PlayerInfo.set_name(TCHAR_TO_UTF8(*Controller->Info.PlayerName));
  		Output.SetNum(PlayerInfo.ByteSizeLong());
  		if (PlayerInfo.SerializeToArray(&Output,Output.Num()))
  		{
  			return true;
  		}
  		return false;
  	}
  	return false;
  	
  }*/

bool UProtobufLibrary::MarshalBroadCast(const FBroadCast& Info, TArray<uint8>& Output)
{
	TSharedPtr<BroadcastStruct> BroadcastStruct = MakeShareable(new struct BroadcastStruct);
	BroadcastStruct->Temp->set_content(Info.Content);
	BroadcastStruct->Temp->set_actiondata(Info.ActionData);
	BroadcastStruct->PlayerInfo->set_charactertype(Info.Info.CharacterType);
	//PlayerInfo.set_name(TCHAR_TO_UTF8(*Info.Info.PlayerName));
	BroadcastStruct->PlayerInfo->set_pid(Info.Info.UserID);
	std::string Temp1 =TCHAR_TO_UTF8(*Info.Info.PlayerName);
	BroadcastStruct->PlayerInfo->set_name(Temp1);
	std::string Temp2 = TCHAR_TO_UTF8(*Info.Info.Token);
	BroadcastStruct->Token->set_token(Temp2);
	BroadcastStruct->PlayerInfo->set_allocated_token(BroadcastStruct->Token);
	BroadcastStruct->MovementStruct->NewLocation->set_x(Info.Movement.Transform.GetLocation().X);
	BroadcastStruct->MovementStruct->NewLocation->set_y(Info.Movement.Transform.GetLocation().Y);
	BroadcastStruct->MovementStruct->NewLocation->set_z(Info.Movement.Transform.GetLocation().Z);
	BroadcastStruct->MovementStruct->NewRotator->set_pitch(Info.Movement.Transform.GetRotation().Rotator().Pitch);
	BroadcastStruct->MovementStruct->NewRotator->set_roll(Info.Movement.Transform.Rotator().Roll);
	BroadcastStruct->MovementStruct->NewRotator->set_yaw(Info.Movement.Transform.Rotator().Yaw);
	BroadcastStruct->MovementStruct->NewScale->set_x(Info.Movement.Transform.GetScale3D().X);
	BroadcastStruct->MovementStruct->NewScale->set_y(Info.Movement.Transform.GetScale3D().Y);
	BroadcastStruct->MovementStruct->NewScale->set_z(Info.Movement.Transform.GetScale3D().Z);
	BroadcastStruct->MovementStruct->NewCRotator->set_pitch(Info.Movement.ControllerRotation.Pitch);
	BroadcastStruct->MovementStruct->NewCRotator->set_yaw(Info.Movement.ControllerRotation.Yaw);
	BroadcastStruct->MovementStruct->NewCRotator->set_roll(Info.Movement.ControllerRotation.Roll);
	BroadcastStruct->MovementStruct->Axis->set_moveforward(Info.Movement.Axis.MoveForward);
	BroadcastStruct->MovementStruct->Axis->set_moveright(Info.Movement.Axis.MoveRight);
	BroadcastStruct->MovementStruct->NewTrans->set_allocated_l(BroadcastStruct->MovementStruct->NewLocation);
	BroadcastStruct->MovementStruct->NewTrans->set_allocated_r(BroadcastStruct->MovementStruct->NewRotator);
	BroadcastStruct->MovementStruct->NewTrans->set_allocated_s(BroadcastStruct->MovementStruct->NewScale);
	BroadcastStruct->MovementStruct->NewInfo->set_allocated_crotation(BroadcastStruct->MovementStruct->NewCRotator);
	BroadcastStruct->MovementStruct->NewInfo->set_allocated_inputaxis(BroadcastStruct->MovementStruct->Axis);
	BroadcastStruct->MovementStruct->NewInfo->set_allocated_trans(BroadcastStruct->MovementStruct->NewTrans);
	BroadcastStruct->Temp->set_allocated_info(BroadcastStruct->MovementStruct->NewInfo);
	BroadcastStruct->Temp->set_allocated_token(BroadcastStruct->PlayerInfo);
	size_t Datalen1 = BroadcastStruct->Temp->ByteSizeLong();
	Output.SetNum(Datalen1);
	if (BroadcastStruct->Temp->SerializeToArray(Output.GetData(),Output.Num()))
	{
		return true;
	}
	return false;
}

bool UProtobufLibrary::UnMarshalBroadCastByMessageType(EMessageType Type, const TArray<uint8>& Input, FBroadCast& Info)
{
	switch (Type)
	{
		case EMessageType::PreLogin:
			UnmarshalToken(Input,Info.Info);
			return true;
		case EMessageType::BroadCast:
			UnmarshalBroadCast(Input,Info);
			return true;
		case EMessageType::SyncPlayer:
			UnmarshalBroadCast(Input,Info);
			return true;
		case EMessageType::KickPlayer:
			UnmarshalBroadCast(Input,Info);
			return true; 
		default:
			return false;
	}
}

bool UProtobufLibrary::UnMarshalChat(const TArray<uint8>& Input, FChat& Chat)
{
	TSharedPtr<Player::PlayerChat> ProtoChat = MakeShareable(new Player::PlayerChat);
	if (ProtoChat)
	{
		if (ProtoChat->ParseFromArray(Input.GetData(),Input.Num()))
		{
			Chat.Type = static_cast<EChatType> (ProtoChat->type());
			Chat.Chat = FString(UTF8_TO_TCHAR(ProtoChat->chat().c_str()));
			const google::protobuf::RepeatedField<int32>& rep_pid= ProtoChat->pid();
			for (auto it_PID = rep_pid.begin();it_PID!=rep_pid.end();it_PID++)
			{
				Chat.PlayerArray.Add(*it_PID);
			}
			return true;
		}
		return false;
	}
	return  false;
	
}

bool UProtobufLibrary::MarshalChat(const FChat& Chat,TArray<uint8>&Output)
{
	Player::PlayerChat ProtoChat;
	for (auto it = Chat.PlayerArray.begin();it!=Chat.PlayerArray.end();++it)
	{
		ProtoChat.add_pid(*it);
	}
	ProtoChat.set_type(int32(Chat.Type));
	std::string Temp =TCHAR_TO_UTF8(*Chat.Chat);
	ProtoChat.set_chat(Temp);
	Output.SetNum(ProtoChat.ByteSizeLong());
	return ProtoChat.SerializeToArray(Output.GetData(),Output.Num());
}

bool UProtobufLibrary::MarshalRayIntersect(const FRayIntersect& RayIntersect, TArray<uint8>& Output)
{
	Player::RayIntersect ProtoRayIntersect;
	Player::Location* Origin = new Player::Location;
	Player::Location* Direction = new Player::Location;
	if (Origin&&Direction)
	{
		Origin->set_x(RayIntersect.Origin.X);
		Origin->set_y(RayIntersect.Origin.Y);
		Origin->set_z(RayIntersect.Origin.Z);
		Direction->set_x(RayIntersect.Direction.X);
		Direction->set_y(RayIntersect.Direction.Y);
		Direction->set_z(RayIntersect.Direction.Z);
	}
	ProtoRayIntersect.set_allocated_origin(Origin);
	ProtoRayIntersect.set_allocated_direction(Direction);
	ProtoRayIntersect.set_actor(TCHAR_TO_UTF8(*RayIntersect.Actor));
	size_t Datalen1 = ProtoRayIntersect.ByteSizeLong();
	Output.SetNum(Datalen1);
	if (ProtoRayIntersect.SerializeToArray(Output.GetData(),Output.Num()))
	{
		return true;
	}
	return false;
	
}

bool UProtobufLibrary::UnMarshalRayIntersect(const TArray<uint8>& Input, FRayIntersect& RayIntersect)
{
	Player::RayIntersect ProtoRayIntersect;
	if (ProtoRayIntersect.ParseFromArray(Input.GetData(),Input.Num()))
	{
		RayIntersect.Origin.X=ProtoRayIntersect.origin().x();
		RayIntersect.Origin.Y=ProtoRayIntersect.origin().y();
		RayIntersect.Origin.Z=ProtoRayIntersect.origin().z();
		RayIntersect.Direction.X=ProtoRayIntersect.direction().x();
		RayIntersect.Direction.Y=ProtoRayIntersect.direction().y();
		RayIntersect.Direction.Z=ProtoRayIntersect.direction().z();
		RayIntersect.Actor= FString(UTF8_TO_TCHAR(ProtoRayIntersect.actor().c_str()));
		return true;
	}
	return false;
}


