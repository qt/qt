import Qt 4.6

Rect {
    id: removeButton
    width: 30
    height: 30
    color: "red"
    radius: 5
    property var expandedWidth: 230
    signal confirmed
    resources: [
        Script {
            function toggle() {
                if (removeButton.state == 'opened') {
                    removeButton.state = '';
                    contacts.mouseGrabbed=false;
                } else {
                    if (!contacts.mouseGrabbed) {
                        removeButton.state = 'opened';
                        contacts.mouseGrabbed=true;
                    }
                }
            }
        
        }
    ]
    Image {
        id: trashIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/trash.png"
        opacity: 1
        MouseRegion {
            anchors.fill: parent
            onClicked: { toggle() }
        }
    }
    Image {
        id: cancelIcon
        width: 22
        height: 22
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        source: "../../shared/pics/cancel.png"
        opacity: 0
        MouseRegion {
            anchors.fill: parent
            onClicked: { toggle() }
        }
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
        MouseRegion {
            anchors.fill: parent
            onClicked: { toggle(); removeButton.confirmed() }
        }
    }
    Text {
        id: text
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: confirmIcon.right
        anchors.leftMargin: 4
        anchors.right: cancelIcon.left
        anchors.rightMargin: 4
        font.bold: true
        color: "white"
        hAlign: "AlignHCenter"
        text: "Remove"
        opacity: 0
    }
    states: [
        State {
            name: "opened"
            SetProperties {
                target: removeButton
                explicit: true
                width: removeButton.expandedWidth
            }
            SetProperties {
                target: text
                opacity: 1
            }
            SetProperties {
                target: confirmIcon
                opacity: 1
            }
            SetProperties {
                target: cancelIcon
                opacity: 1
            }
            SetProperties {
                target: trashIcon
                opacity: 0
            }
        }
    ]
    transitions: [
        Transition {
            fromState: "*"
            toState: "opened"
            reversible: true
            NumberAnimation {
                properties: "opacity,x,width"
                duration: 200
            }
        }
    ]
}
