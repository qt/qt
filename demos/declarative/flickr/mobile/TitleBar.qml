import Qt 4.6

Item {
    id: TitleBar
    property string untaggedString: "Uploads from everyone"
    property string taggedString: "Recent uploads tagged "

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: Container
        width: (parent.width * 2) - 55 ; height: parent.height

        Script {
            function accept() {
                TitleBar.state = ""
                Background.state = ""
                RssModel.tags = Editor.text
            }
        }

        Text {
            id: CategoryText
            anchors {
                left: parent.left; right: TagButton.left; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            elide: "ElideLeft"
            text: (RssModel.tags=="" ? untaggedString : taggedString + RssModel.tags)
            font.bold: true; color: "White"; style: "Raised"; styleColor: "Black"
        }

        Button {
            id: TagButton; x: TitleBar.width - 50; width: 45; height: 32; text: "..."
            onClicked: if (TitleBar.state == "Tags") accept(); else TitleBar.state = "Tags"
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            id: LineEdit
            y: 4; height: parent.height - 9
            anchors { left: TagButton.right; leftMargin: 5; right: parent.right; rightMargin: 5 }

            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            TextInput {
                id: Editor
                anchors {
                    left: parent.left; right: parent.right; leftMargin: 10; rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                cursorVisible: true; font.bold: true
                color: "#151515"; selectionColor: "Green"
            }

            Keys.forwardTo: [ (ReturnKey), (Editor)]

            Item {
                id: ReturnKey
                Keys.onReturnPressed: accept()
                Keys.onEscapePressed: TitleBar.state = ""
            }
        }
    }

    states: State {
        name: "Tags"
        PropertyChanges { target: Container; x: -TagButton.x + 5 }
        PropertyChanges { target: TagButton; text: "OK" }
        PropertyChanges { target: LineEdit; focus: true }
    }

    transitions: Transition {
        NumberAnimation { properties: "x"; easing: "easeInOutQuad" }
    }
}
