import Qt 4.6

Item {
    id: Toolbar
    property var button2: Button2

    BorderImage { source: "images/titlebar2.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Button { anchors.left: parent.left; anchors.leftMargin: 5; y: 3; width: 140; height: 32; text: "Update"; onClicked: RssModel.reload() }

    Button {
        id: Button2
        anchors.right: parent.right; anchors.rightMargin: 5; y: 3; width: 140; height: 32; text: "View mode"
        onClicked: {
            if (Button2.text == "View mode") {
                if (Screen.inListView == true)
                Screen.inListView = false;
                else
                Screen.inListView = true
            }
        }
    }
}
