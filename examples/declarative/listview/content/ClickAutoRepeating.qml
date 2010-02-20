import Qt 4.6

Item {
    id: page
    property int repeatdelay: 300
    property int repeatperiod: 75
    property bool isPressed: false

    signal pressed
    signal released
    signal clicked

    isPressed: SequentialAnimation {
        running: false
        id: autoRepeat
        PropertyAction { target: page; property: "isPressed"; value: true }
        ScriptAction { script: page.pressed() }
        ScriptAction { script: page.clicked() }
        PauseAnimation { duration: repeatdelay }
        SequentialAnimation {
            repeat: true
            ScriptAction { script: page.clicked() }
            PauseAnimation { duration: repeatperiod }
        }
    }
    MouseRegion {
        anchors.fill: parent
        onPressed: autoRepeat.start()
        onReleased: { autoRepeat.stop(); parent.isPressed = false; page.released() }
    }
}
