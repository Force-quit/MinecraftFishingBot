function Component()
{
    
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    component.addOperation("CreateShortcut", "@TargetDir@/MinecraftFishingBot.exe", "@StartMenuDir@/MinecraftFishingBot.lnk", "workingDirectory=@TargetDir@");
}