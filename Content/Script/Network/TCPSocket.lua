--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_HASTcpSocketConnection_C
local M = UnLua.Class()


function M:OnReceivedMessage(ConnectionID,Output)
    self.Message = Output
    self:ProcessMessage(self.Message)
end

function M:OnSendChat(Chat)
    self:SendChatToServer(Chat)
end
function M:OnTick(BroadCast)
    self:SendBroadcastToServer(UE.EMessageType.BroadCast,BroadCast)
end
-- function M:Initialize(Initializer)
-- end

-- function M:UserConstructionScript()
-- end

-- function M:ReceiveBeginPlay()
-- end

-- function M:ReceiveEndPlay()
-- end

-- function M:ReceiveTick(DeltaSeconds)
-- end

-- function M:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
-- end

-- function M:ReceiveActorBeginOverlap(OtherActor)
-- end

-- function M:ReceiveActorEndOverlap(OtherActor)
-- end

return M
