
function OnNativeMessage(text) {
    Core.FireEvent(JavaScriptTest.Events.ScriptMessage, 0, "Hello from JavaScript!");
}