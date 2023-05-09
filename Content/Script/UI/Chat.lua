--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type UI_Chat_C
local M = UnLua.Class()

local switch=0
local bVisible = false
local bTextEmpty = true
local UIDArray = UE.TArray(0)
local PlayerWidgetMap = UE.TMap(0,UE.UWidget)
--要发送的人的数据
local SendPlayerInfo
local InputWidget
local PlayerController
function M:SwitchInputBox(Index)
    if Index == 0 then
        if SendPlayerInfo ~= nil then
            InputWidget:SetTypeContext(SendPlayerInfo.PlayerName)
        else
            InputWidget:SetTypeContext("未选择玩家")
        end
    elseif Index == 1 then
        InputWidget:SetTypeContext("小队")
    elseif Index == 2 then
        InputWidget:SetTypeContext("广播")
    end
end

function M:AddChat(Username,Chat)
    local ChildClass = UE.UClass.Load("/Game/UI/UI_ChatChild.UI_ChatChild_C")
    local Child = UE.UWidgetBlueprintLibrary.Create(self,ChildClass)
    self.ScrollBox_Chat:AddChild(Child)
    Child:SetContext(Username,Chat)
end

function M:AddPlayer(PlayerInfo)
    local ChildClass = UE.UClass.Load("/Game/UI/UI_Player.UI_Player_C")
    local Child = UE.UWidgetBlueprintLibrary.Create(self,ChildClass)
    print(UE.UKismetSystemLibrary.GetDisplayName(ChildClass))
    self.ScrollBox_Select:AddChild(Child)
    Child:SetContext(PlayerInfo)
    PlayerWidgetMap:Add(PlayerInfo.UserID,Child)
    Child.ChatUI = self
end

function M:RemovePlayer(PlayerInfo)
    local FoundedWidget = PlayerWidgetMap:Find(PlayerInfo.UserID)
    if FoundedWidget then
        FoundedWidget:RemoveFromParent()
    end
end

function M:SelectPlayer(PlayerInfo)
    SendPlayerInfo = PlayerInfo
    InputWidget:SetTypeContext(PlayerInfo.PlayerName)
    switch = 0
end

function M:UndoSelect()
    SendPlayerInfo = nil
    switch=0
    M:SwitchInputBox(0)
end



function M:SwitchChat()
    self.WidgetSwitcher_Chat:SetActiveWidgetIndex(0)
end

function M:SwitchSelect()
    self.WidgetSwitcher_Chat:SetActiveWidgetIndex(1)
end

function M:PressTab()
    if switch==2 then
        switch = 0 
        self:SwitchInputBox(switch)
    else
    switch=switch+1
    self:SwitchInputBox(switch)  
    end
end

function M:PressEnter()
    if bVisible==false then
        self:SetVisibility(UE.ESlateVisibility.Visible)
        PlayerController.bShowMouseCursor = true
        bVisible=true
    elseif bVisible==true then
        if bTextEmpty==false then
            bTextEmpty=true
            UIDArray:Add(self.PlayerID)
            UIDArray:Add(SendPlayerInfo.UserID)
            self:SendChat(self.ChatMessage,switch,UIDArray)
            self.UI_ChatInputBox.EditableText_Chat:SetText("")
            UIDArray:Clear()
        elseif bTextEmpty==true then
            self:SetVisibility(UE.ESlateVisibility.Hidden)
            PlayerController.bShowMouseCursor = false
            bVisible=false
        end
    end
end


function M:ChatTextChanged(Text)
    bTextEmpty = UE.UKismetTextLibrary.TextIsEmpty(Text)
    self.ChatMessage = Text
    print(self.ChatMessage)
end

function M:Construct()
    InputWidget=self.UI_ChatInputBox
    self:SwitchInputBox()
    self:ListenForInputAction("SwitchInput",UE.EInputEvent.IE_Pressed,true, function() self:PressTab() end)
    self:ListenForInputAction("SendMessage",UE.EInputEvent.IE_Pressed,true, function() self:PressEnter() end)
    self:SetVisibility(UE.ESlateVisibility.Hidden)
    local FatherPC = UE.UGameplayStatics.GetPlayerController(self,0)
    local PCClass = UE.UClass.Load("/Game/BP_AHASPlayerController.BP_AHASPlayerController_C")
    PlayerController = FatherPC:Cast(PCClass)
    self.UI_ChatInputBox.EditableText_Chat.OnTextChanged:Add(self,self.ChatTextChanged)
    self.Button_Chat.OnClicked:Add(self,self.SwitchChat)
    self.Button_Select.OnClicked:Add(self,self.SwitchSelect)
    self:SwitchInputBox(0) 
end


--function M:Initialize(Initializer)
--end

--function M:PreConstruct(IsDesignTime)
--end

--function M:Tick(MyGeometry, InDeltaTime)
--end

return M
