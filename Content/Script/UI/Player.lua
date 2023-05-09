--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type UI_Player_C
local M = UnLua.Class()

function M:SetContext(PlayerInfo)
    self.PlayerInfo = PlayerInfo
    self.EditableText_Username:SetText(PlayerInfo.PlayerName)
    self.EditableText_Username:SetIsReadOnly(true)
end
--function M:Initialize(Initializer)
--end

--function M:PreConstruct(IsDesignTime)
--end
function M:UndoSelect()
    self.ChatUI:UndoSelect()
end

function M:SelectPlayer()
    self.ChatUI:SelectPlayer(self.PlayerInfo)
end

function M:Construct()
    self.Button_No.OnClicked:Add(self,self.UndoSelect)
    self.Button_OK.OnClicked:Add(self,self.SelectPlayer)
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
