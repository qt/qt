Rect {
    id: removeButton
    width: 30
    height: 30
    color: "red"
    radius: 5
    resources: [
        Script {
            function toggle() {
                if (removeButton.state == 'opened') {
                    removeButton.state = '';
                } else {
                    removeButton.state = 'opened';
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
            onClicked: { toggle() }
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
            SetProperty {
                target: removeButton
                property: "width"
                value: 230
            }
            SetProperty {
                target: text
                property: "opacity"
                value: 1
            }
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
                target: trashIcon
                property: "opacity"
                value: 0
            }
        }
    ]
//! [transition]
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
//! [transition]
}
