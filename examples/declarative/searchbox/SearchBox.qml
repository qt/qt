import Qt 4.6

FocusScope {
    id: focusScope
    width: 250; height: 28

    BorderImage {
        source: "images/lineedit-bg.png"
        width: parent.width; height: parent.height
        border.left: 4; border.top: 4; border.right: 4; border.bottom: 4
    }

    BorderImage {
        source: "images/lineedit-bg-focus.png"
        width: parent.width; height: parent.height
        border.left: 4; border.top: 4; border.right: 4; border.bottom: 4
        visible: parent.wantsFocus ? true : false
    }

    Text {
        anchors.fill: parent; anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: "Type something..."; color: "gray"; font.italic: true
        opacity: textInput.text == "" ? 1 : 0
        //opacity: Behavior { NumberAnimation { duration: 200 } }
    }

    MouseRegion { anchors.fill: parent; onClicked: focusScope.focus = true }


    TextInput {
        id: textInput
        anchors.left: parent.left; anchors.leftMargin: 8
        //anchors.right: clear.left
        anchors.verticalCenter: parent.verticalCenter
        focus: if(1) true
    }
    Image {
        id: clear
        anchors.right: parent.right; anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        source: "images/edit-clear-locationbar-rtl.png"
        opacity: textInput.text == "" ? 0 : 1
        //opacity: Behavior { NumberAnimation { duration: 200 } }
        MouseRegion { anchors.fill: parent; onClicked: { textInput.text = ''; focusScope.focus = true } }
    }
}
