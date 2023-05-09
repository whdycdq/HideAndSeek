--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type UI_Login_C
local M = UnLua.Class()
function M:OnUsernameChanged(Text)
    self.Username = UE.UKismetTextLibrary.Conv_TextToString(Text)
end


function M:OnPasswordChanged(Text)
    self.Password = UE.UKismetTextLibrary.Conv_TextToString(Text)
end

function M:OnClickedOK()
    UE.UGameplayStatics.GetGameInstance();
    self.ServerBaseUrl= "http://127.0.0.1:8080/"
    print(self.ServerBaseUrl)
    self:UserLogin(self.Username,self.Password)
end

function M:OnClickedNo()

end
--function M:Initialize(Initializer)
--    self.ServerBaseUrl = UE.UKismetTextLibrary.Conv_TextToString(self.EditableText_Http.GetText())
--end

--function M:PreConstruct(IsDesignTime)
--end

function M:Construct()
    self.EditableText_Username.OnTextChanged:Add(self,self.OnUsernameChanged)
    self.EditableText_Password.OnTextChanged:Add(self,self.OnPasswordChanged)
    self.Button_OK.OnClicked:Add(self,self.OnClickedOK)
    self.Button_no.OnClicked:Add(self,self.OnClickedNo)
end

--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
