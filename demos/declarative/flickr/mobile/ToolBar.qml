import Qt 4.6

Item {
    id: Toolbar

    property var button1Label
    property var button2Label
    signal button1Clicked
    signal button2Clicked

    BorderImage { source: "images/titlebar2.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Button {
        anchors.left: parent.left; anchors.leftMargin: 5; y: 3; width: 140; height: 32; text: button1Label
        onClicked: Toolbar.button1Clicked()
    }

    Button {
        anchors.right: parent.right; anchors.rightMargin: 5; y: 3; width: 140; height: 32; text: button2Label
        onClicked: Toolbar.button2Clicked()
    }
}
