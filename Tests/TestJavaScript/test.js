
function OnNativeMessage(text) {
    Core.FireEvent(JavaScriptTest.ScriptMessage, 0, "Hello from JavaScript!");
}