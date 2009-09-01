import Qt 4.6

Item {
    id: Toolbar

    property alias button1Label: Button1.text
    property alias button2Label: Button2.text
    signal button1Clicked
    signal button2Clicked

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Button {
        id: Button1
        anchors.left: parent.left; anchors.leftMargin: 5; y: 3; width: 140; height: 32
        onClicked: Toolbar.button1Clicked()
    }

    Button {
        id: Button2
        anchors.right: parent.right; anchors.rightMargin: 5; y: 3; width: 140; height: 32
        onClicked: Toolbar.button2Clicked()
    }
}
