function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/MinecraftFishingBot.exe", "@StartMenuDir@/MinecraftFishingBot.lnk",
            "workingDirectory=@TargetDir@");
    }
}