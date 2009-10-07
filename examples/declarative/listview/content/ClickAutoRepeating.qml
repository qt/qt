import Qt 4.6

Item {
    id: page
    property int repeatdelay: 300
    property int repeatperiod: 75
    property bool pressed: false
    signal pressed
    signal released
    signal clicked
    pressed: SequentialAnimation {
        id: autoRepeat
        PropertyAction { target: page; property: "pressed"; value: true }
        ScriptAction { script: page.onPressed }
        ScriptAction { script: page.onClicked }
        PauseAnimation { duration: repeatdelay }
        SequentialAnimation {
            repeat: true
            ScriptAction { script: page.onClicked }
            PauseAnimation { duration: repeatperiod }
        }
    }
    MouseRegion {
        anchors.fill: parent
        onPressed: autoRepeat.start()
        onReleased: { autoRepeat.stop(); parent.pressed = false; page.released }
    }
}
