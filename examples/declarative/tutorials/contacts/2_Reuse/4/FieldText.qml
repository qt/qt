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
                if (!contactDetails.mouseGrabbed) {
                    fieldText.state='editing';
                    contactDetails.mouseGrabbed=true;
                }
            }
            function confirm() {
                fieldText.text = textEdit.text;
                fieldText.state='';
                contactDetails.mouseGrabbed=false;
                fieldText.confirmed.emit();
            }
            function reset() {
                textEdit.text = fieldText.text;
                fieldText.state='';
                contactDetails.mouseGrabbed=false;
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
            SetProperty {
                target: confirmIcon
                property: "opacity"
                value: 1
            }
            SetProperty {
                target: cancelIcon
                property: "opacity"
                value: 1
            }
            SetProperty {
                target: textEdit
                property: "readOnly"
                value: false
            }
            SetProperty {
                target: textEdit
                property: "focus"
                value: true
            }
            SetProperty {
                target: editRegion
                property: "opacity"
                value: 0
            }
            SetProperty {
                target: textEdit.anchors
                property: "leftMargin"
                value: 39
            }
            SetProperty {
                target: textEdit.anchors
                property: "rightMargin"
                value: 39
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
