import Qt 4.6

Rect {
    id: fieldText
    height: 30
    radius: 5
    color: "white"
    property var text: ""
    property var label: ""
    onTextChanged: { reset() }
    signal confirmed
    resources: [
        Script {

            function edit() {
                if (!contacts.mouseGrabbed) {
                    fieldText.state='editing';
                    contacts.mouseGrabbed=true;
                }
            }
            function confirm() {
                fieldText.text = textEdit.text;
                fieldText.state='';
                contacts.mouseGrabbed=false;
                fieldText.confirmed();
            }
            function reset() {
                textEdit.text = fieldText.text;
                fieldText.state='';
                contacts.mouseGrabbed=false;
            }
        
        }
    ]
    Image {
        id: cancelIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/cancel.png"
        opacity: 0
    }
    Image {
        id: confirmIcon
        width: 22
        height: 22
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/ok.png"
        opacity: 0
    }
    TextEdit {
        id: textEdit
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        color: "black"
        font.bold: true
        readOnly: true
        wrap: false
    }
    Text {
        id: textLabel
        x: 5
        width: parent.width-10
        anchors.verticalCenter: parent.verticalCenter
        hAlign: "AlignHCenter"
        color: "#505050"
        font.italic: true
        text: fieldText.label
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
            SetProperties {
                target: confirmIcon
                opacity: 1
            }
            SetProperties {
                target: cancelIcon
                opacity: 1
            }
            SetProperties {
                target: textEdit
                readOnly: false
            }
            SetProperties {
                target: textEdit
                focus: true
            }
            SetProperties {
                target: editRegion
                opacity: 0
            }
            SetProperties {
                target: textEdit.anchors
                leftMargin: 39
            }
            SetProperties {
                target: textEdit.anchors
                rightMargin: 39
            }
        }
    ]
    transitions: [
        Transition {
            fromState: ""
            toState: "*"
            reversible: true
            NumberAnimation {
                properties: "opacity,leftMargin,rightMargin"
                duration: 200
            }
            ColorAnimation {
                property: "color"
                duration: 150
            }
        }
    ]
}
