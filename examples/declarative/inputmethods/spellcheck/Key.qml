import QtQuick 2.0

Rectangle {
    property string text
    property string displayText
    property alias font: keyText.font
    property int key: 0

    id: root
    radius: 2

    width: 28
    height: 28

    gradient: Gradient {
        GradientStop { position: 0.0; color: "darkgrey" }
        GradientStop { position: 1.0; color: "grey" }
    }

    Text {
        id: keyText

        anchors.fill: parent

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        font.pixelSize: 18
        font.capitalization: keyboard.shift && displayText == "" ? Font.AllUppercase : Font.MixedCase

        text: root.displayText != "" ? root.displayText : root.text

        style: !mouseArea.pressed ? Text.Raised : Text.Normal
        color: "white"
        styleColor: "grey"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onPressed: keyboard.keyPress(key, text)
        onReleased: keyboard.keyRelease(key, text)
    }
}
