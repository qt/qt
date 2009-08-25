import Qt 4.6

Item {
    id: TitleBar

    BorderImage { source: "images/titlebar2.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: Container
        width: (parent.width * 2) - 55 ; height: parent.height

        Script {
            function accept() {
                RssModel.tags = Editor.text
                TitleBar.state = ""
            }
        }

        Text {
            id: CategoryText
            anchors.left: parent.left; anchors.right: TagButton.left
            anchors.leftMargin: 10; anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            elide: "ElideLeft"
            text: (RssModel.tags=="" ? "Uploads from everyone" : "Recent Uploads tagged " + RssModel.tags)
            font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
        }

        Button {
            id: TagButton; x: TitleBar.width - 50; y: 3; width: 45; height: 32; text: "..."
            onClicked: if (TitleBar.state == "Tags") accept(); else TitleBar.state = "Tags"
        }

        Item {
            id: LineEdit
            anchors.left: TagButton.right; anchors.leftMargin: 5; y: 4
            anchors.right: parent.right; anchors.rightMargin: 5; height: parent.height - 9
            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            TextInput {
                id: Editor
                anchors.left: parent.left; anchors.right: parent.right
                anchors.leftMargin: 10; anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                cursorVisible: true; font.bold: true
                color: "#151515"; highlightColor: "green"
            }
            KeyProxy {
                id: Proxy
                anchors.fill: parent
                targets: [(ReturnKey), (Editor)]
            }
            Item {
                id: ReturnKey
                Keys.onReturnPressed: accept()
                Keys.onEscapePressed: TitleBar.state = ""
            }
        }
    }
    states: [
        State {
            name: "Tags"
            PropertyChanges { target: Container; x: -TagButton.x + 5 }
            PropertyChanges { target: TagButton; text: "OK" }
            PropertyChanges { target: Proxy; focus: true }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "*"
            NumberAnimation { properties: "x"; easing: "easeInOutQuad" }
        }
    ]
}
