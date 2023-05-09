--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_HideAndSeekGameMode_C
local M = UnLua.Class()
-- function M:Initialize(Initializer)
-- end

-- function M:UserConstructionScript()
-- end


function M:OnReceivedChat(Chat)
    local temp = self.WidgetController:GetChatWidget()
    if temp then
        if Chat.PlayerArray:Get(1) == 0 then
            temp:AddChat("Server",Chat.Chat)
        elseif Chat.PlayerArray:Get(1) ~=0 then
            local PC = self.PCMap:Find(Chat.PlayerArray:Get(1))
            temp:AddChat(PC.Info.PlayerName,Chat.Chat)
        end
    end
end


function M:OnReceivedBroadCast(Type,Broadcast)
    if Type==UE.EMessageType.PreLogin then
        self.PreLogin(self,Broadcast)
        local temp = self.WidgetController:GetChatWidget()
        temp.PlayerID = Broadcast.Info.UserID
        print(temp.PlayerID)
    end
    if Type==UE.EMessageType.SyncPlayer then
        self.SyncPlayer(self,Broadcast)
        local temp = self.WidgetController:GetChatWidget()
        temp:AddPlayer(Broadcast.Info)
    end
    if Type==UE.EMessageType.BroadCast then
        self.Broadcast(self,Broadcast)
    end
    if Type== UE.EMessageType.KickPlayer then
        local temp = self.WidgetController:GetChatWidget()
        temp:RemovePlayer(Broadcast.Info)
        self.KickPlayer(self,Broadcast)
    end
end


function M:ReceiveBeginPlay()
    print("hello from patcher")
    print("hello from 1.1")
end

-- function M:ReceiveEndPlay()
-- end

function M:ReceiveTick(DeltaSeconds)
    self:Tick(DeltaSeconds)
end


function M:OnLoginResponseReceived(BroadCast)
    self.SocketConnection:ConnectToServer("127.0.0.1",7777, self.SocketConnection.ConnectionID)
    --self.PreLogin(self,BroadCast)
    self.SpawnBC = BroadCast
end
function M:OnTCPConnected(ConnectionID)
   -- self.PreLogin(self,Broadcast)
   local temp = self.WidgetController:GetLoginWidget()
   temp:HideUI()
   self.PreLogin(self,self.SpawnBC)
end
-- function M:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
-- end

-- function M:ReceiveActorBeginOverlap(OtherActor)
-- end

-- function M:ReceiveActorEndOverlap(OtherActor)
-- end

return M
