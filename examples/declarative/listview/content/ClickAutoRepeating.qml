import Qt 4.7

Item {
    id: page
    property int repeatdelay: 300
    property int repeatperiod: 75
    property bool isPressed: false

    signal pressed
    signal released
    signal clicked

    SequentialAnimation on isPressed {
        running: false
        id: autoRepeat
        PropertyAction { target: page; property: "isPressed"; value: true }
        ScriptAction { script: page.pressed() }
        ScriptAction { script: page.clicked() }
        PauseAnimation { duration: repeatdelay }
        SequentialAnimation {
            loops: Animation.Infinite
            ScriptAction { script: page.clicked() }
            PauseAnimation { duration: repeatperiod }
        }
    }
    MouseArea {
        anchors.fill: parent
        onPressed: autoRepeat.start()
        onReleased: { autoRepeat.stop(); parent.isPressed = false; page.released() }
    }
}
