import Qt 4.7

FocusScope {
    id: focusScope
    width: 250; height: 28

    BorderImage {
        source: "images/lineedit-bg.png"
        width: parent.width; height: parent.height
        border { left: 4; top: 4; right: 4; bottom: 4 }
    }

    BorderImage {
        source: "images/lineedit-bg-focus.png"
        width: parent.width; height: parent.height
        border { left: 4; top: 4; right: 4; bottom: 4 }
        visible: parent.wantsFocus ? true : false
    }

    Text {
        id: typeSomething
        anchors.fill: parent; anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: "Type something..."
        color: "gray"
        font.italic: true
    }

    MouseArea { anchors.fill: parent; onClicked: focusScope.focus = true }

    TextInput {
        id: textInput
        anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
        focus: true
    }

    Image {
        id: clear
        anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
        source: "images/edit-clear-locationbar-rtl.png"
        opacity: 0

        MouseArea { 
            anchors.fill: parent
            onClicked: { textInput.text = ''; focusScope.focus = true }
        }
    }

    states: State {
        name: "hasText"; when: textInput.text != ''
        PropertyChanges { target: typeSomething; opacity: 0 }
        PropertyChanges { target: clear; opacity: 1 }
    }

    transitions: [
        Transition {
            from: ""; to: "hasText"
            NumberAnimation { exclude: typeSomething; properties: "opacity" }
        },
        Transition {
            from: "hasText"; to: ""
            NumberAnimation { properties: "opacity" }
        }
    ]
}
