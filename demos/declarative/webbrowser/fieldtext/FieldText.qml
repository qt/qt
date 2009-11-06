import Qt 4.6

Item {
    id: fieldText
    height: 30
    property string text: ""
    property string label: ""
    property bool mouseGrabbed: false
    signal confirmed
    signal cancelled
    signal startEdit

    Script {

        function edit() {
            if (!mouseGrabbed) {
                fieldText.startEdit();
                fieldText.state='editing';
                mouseGrabbed=true;
            }
        }

        function confirm() {
            fieldText.state='';
            fieldText.text = textEdit.text;
            mouseGrabbed=false;
            fieldText.confirmed();
        }

        function reset() {
            textEdit.text = fieldText.text;
            fieldText.state='';
            mouseGrabbed=false;
            fieldText.cancelled();
        }

    }

    Image {
        id: cancelIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "pics/cancel.png"
        opacity: 0
    }

    Image {
        id: confirmIcon
        width: 22
        height: 22
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "pics/ok.png"
        opacity: 0
    }

    TextInput {
        id: textEdit
        text: fieldText.text
        focus: false
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        color: "black"
        font.bold: true
        readOnly: true
        onAccepted: confirm()
        Keys.onEscapePressed: reset()
    }

    Text {
        id: textLabel
        x: 5
        width: parent.width-10
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        color: fieldText.state == "editing" ? "#505050" : "#AAAAAA"
        font.italic: true
        font.bold: true
        text: label
        opacity: textEdit.text == '' ? 1 : 0
        opacity: Behavior {
            NumberAnimation {
                property: "opacity"
                duration: 250
            }
        }
    }

    MouseRegion {
        anchors.fill: cancelIcon
        onClicked: { reset() }
    }

    MouseRegion {
        anchors.fill: confirmIcon
        onClicked: { confirm() }
    }

    MouseRegion {
        id: editRegion
        anchors.fill: textEdit
        onClicked: { edit() }
    }

    states: [
    State {
        name: "editing"
        PropertyChanges {
            target: confirmIcon
            opacity: 1
        }
        PropertyChanges {
            target: cancelIcon
            opacity: 1
        }
        PropertyChanges {
            target: textEdit
            color: "black"
            readOnly: false
            focus: true
            selectionStart: 0
            selectionEnd: -1
        }
        PropertyChanges {
            target: editRegion
            opacity: 0
        }
        PropertyChanges {
            target: textEdit.anchors
            leftMargin: 34
        }
        PropertyChanges {
            target: textEdit.anchors
            rightMargin: 34
        }
    }
    ]

    transitions: [
    Transition {
        from: ""
        to: "*"
        reversible: true
        NumberAnimation {
            matchProperties: "opacity,leftMargin,rightMargin"
            duration: 200
        }
        ColorAnimation {
            property: "color"
            duration: 150
        }
    }
    ]
}
