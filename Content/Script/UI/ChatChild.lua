--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type UI_ChatChild_C
local M = UnLua.Class()
local switch=0
function M:SetContext(Username,Chat)
    self.EditableText_Chat:SetText(Chat)
    self.EditableText_Chat:SetIsReadOnly(true)
    self.EditableText_Username:SetText(Username)
    self.EditableText_Username:SetIsReadOnly(true)
    self.EditableText_Chat.WidgetStyle = self.ChatTextStyle
    self.EditableText_Username.WidgetStyle = self.UsernameTextStyle
end

function M:SetTypeContext(Context)
    self.EditableText_Username:SetText(Context)
    self.EditableText_Username:SetIsReadOnly(true)
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
