function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/AutoClicker.exe", "@StartMenuDir@/AutoClicker.lnk",
            "workingDirectory=@TargetDir@");
    }
}