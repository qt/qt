import Qt 4.6

Item {
    focus: true
    Keys.onPressed: keysTestObject.keyPress(event.key, event.text, event.modifiers)
    Keys.onReleased: { keysTestObject.keyRelease(event.key, event.text, event.modifiers); event.accepted = true; }
    Keys.onReturnPressed: keysTestObject.keyPress(event.key, "Return", event.modifiers)
    Keys.forwardTo: [ item2 ]

    Item {
        id: item2
        Keys.onPressed: keysTestObject.forwardedKey(event.key)
        Keys.onReleased: keysTestObject.forwardedKey(event.key)
    }
}
