import Qt 4.7

Item {
    id: titleBar
    property string untaggedString: "Uploads from everyone"
    property string taggedString: "Recent uploads tagged "

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }

    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        function accept() {
            titleBar.state = ""
            background.state = ""
            rssModel.tags = editor.text
        }

        Text {
            id: categoryText
            anchors {
                left: parent.left; right: tagButton.left; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            elide: Text.ElideLeft
            text: (rssModel.tags=="" ? untaggedString : taggedString + rssModel.tags)
            font.bold: true; color: "White"; style: Text.Raised; styleColor: "Black"
        }

        Button {
            id: tagButton; x: titleBar.width - 50; width: 45; height: 32; text: "..."
            onClicked: if (titleBar.state == "Tags") container.accept(); else titleBar.state = "Tags"
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            id: lineEdit
            y: 4; height: parent.height - 9
            anchors { left: tagButton.right; leftMargin: 5; right: parent.right; rightMargin: 5 }

            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            TextInput {
                id: editor
                anchors {
                    left: parent.left; right: parent.right; leftMargin: 10; rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                cursorVisible: true; font.bold: true
                color: "#151515"; selectionColor: "Green"
            }

            Keys.forwardTo: [ (returnKey), (editor)]

            Item {
                id: returnKey
                Keys.onReturnPressed: container.accept()
                Keys.onEscapePressed: titleBar.state = ""
            }
        }
    }

    states: State {
        name: "Tags"
        PropertyChanges { target: container; x: -tagButton.x + 5 }
        PropertyChanges { target: tagButton; text: "OK" }
        PropertyChanges { target: lineEdit; focus: true }
    }

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: "InOutQuad" }
    }
}
