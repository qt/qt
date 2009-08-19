import Qt 4.6

Rect {
    id: removeButton
    width: 30
    height: 30
    color: "red"
    radius: 5
//! [script]
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
//! [script]
//! [mouse region]
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
            id: trashMouseRegion
            anchors.fill: parent
        }
        Connection {
            sender: trashMouseRegion
            signal: clicked()
            script: {
                toggle()
            }
        }
    }
//! [mouse region]
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
        horizontalAlignment: "AlignHCenter"
        text: "Remove"
        opacity: 0
    }
//! [states]
    states: [
        State {
            name: "opened"
            SetProperties {
                target: removeButton
                width: 230
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
//! [states]
}
