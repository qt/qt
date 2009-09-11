import Qt 4.6

Item {
    id: Page
    property int repeatdelay: 300
    property int repeatperiod: 75
    property bool pressed: false
    signal pressed
    signal released
    signal clicked
    pressed: SequentialAnimation {
        id: AutoRepeat
        PropertyAction { target: Page; property: "pressed"; value: true }
        ScriptAction { script: Page.onPressed }
        ScriptAction { script: Page.onClicked }
        PauseAnimation { duration: repeatdelay }
        SequentialAnimation {
            repeat: true
            ScriptAction { script: Page.onClicked }
            PauseAnimation { duration: repeatperiod }
        }
    }
    MouseRegion {
        id: MR
        anchors.fill: parent
        onPressed: AutoRepeat.start()
        onReleased: { AutoRepeat.stop(); parent.pressed = false; Page.released }
    }
}
