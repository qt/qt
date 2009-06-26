Rect {
    id: fieldText
    height: 30
    radius: 5
    color: "black"
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
                fieldText.confirmed.emit();
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
    TextEdit {
        id: textEdit
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        color: "white"
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
        color: contactDetails.state == "editing" ? "#505050" : "#AAAAAA"
        font.italic: true
        font.bold: true
        text: fieldText.label
        opacity: textEdit.text == '' ? 1 : 0
        opacity: Behavior {
            NumericAnimation {
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
                target: fieldText
                color: "white"
            }
            SetProperties {
                target: textEdit
                color: "black"
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
                leftMargin: 34
            }
            SetProperties {
                target: textEdit.anchors
                rightMargin: 34
            }
        }
    ]
    transitions: [
        Transition {
            fromState: ""
            toState: "*"
            reversible: true
            NumericAnimation {
                properties: "opacity,leftMargin,rightMargin"
                duration: 200
            }
            ColorAnimation {
                duration: 150
            }
        }
    ]
}
