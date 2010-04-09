import Qt 4.7

Item {
    id: titleBar

    signal update()
    onYChanged: state="" //When switching titlebars

    BorderImage { source: "images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }
    Item {
        id: container
        width: (parent.width * 2) - 55 ; height: parent.height

        function accept() {
            if(rssModel.authName == '' || rssModel.authPass == '')
                return false;//Can't login like that

            var postData = "status=" + editor.text;
            var postman = new XMLHttpRequest();
            postman.open("POST", "http://twitter.com/statuses/update.xml", true, rssModel.authName,  rssModel.authPass);
            postman.onreadystatechange = function() {
                if (postman.readyState == postman.DONE) {
                    titleBar.update();
                }
            }
            postman.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            postman.send(postData);

            editor.text = ""
            titleBar.state = ""
        }

        Rectangle {
            x: 6; width: 50; height: 50; color: "white"; smooth: true
            anchors.verticalCenter: parent.verticalCenter

            UserModel { user: rssModel.authName; id: userModel }
            Component { id: imgDelegate;
                Item {
                    Loading { width:48; height:48; visible: realImage.status != 1 }
                    Image { source: image; width:48; height:48; id: realImage }
                }
            }
            ListView { model: userModel.model; x:1; y:1; delegate: imgDelegate }
        }

        Text {
            id: categoryText
            anchors.left: parent.left; anchors.right: tagButton.left
            anchors.leftMargin: 58; anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideLeft
            text: "Timeline for " + rssModel.authName
            font.pixelSize: 12; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
        }

        Button {
            id: tagButton; x: titleBar.width - 90; width: 85; height: 32; text: "New Post..."
            anchors.verticalCenter: parent.verticalCenter;
            onClicked: if (titleBar.state == "Posting") container.accept(); else titleBar.state = "Posting"
        }

        Text {
            id: charsLeftText; anchors.horizontalCenter: tagButton.horizontalCenter;
            anchors.top: tagButton.bottom; anchors.topMargin: 2
            text: {140 - editor.text.length;} visible: titleBar.state == "Posting"
            font.pointSize: 10; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
        }
        Item {
            id: txtEdit;
            anchors.left: tagButton.right; anchors.leftMargin: 5; y: 4
            anchors.right: parent.right; anchors.rightMargin: 40; height: parent.height - 9
            BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }

            Binding {//TODO: Can this be a function, which also resets the cursor? And flashes?
                when: editor.text.length > 140
                target: editor
                property: "text"
                value: editor.text.slice(0,140)
            }
            TextEdit {
                id: editor
                anchors.left: parent.left;
                anchors.leftMargin: 8;
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4;
                cursorVisible: true; font.bold: true
                width: parent.width - 12
                height: parent.height - 8
                font.pointSize: 10
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
                color: "#151515"; selectionColor: "green"
            }
            Keys.forwardTo: [(returnKey), (editor)]
            Item {
                id: returnKey
                Keys.onReturnPressed: container.accept()
                Keys.onEscapePressed: titleBar.state = ""
            }
        }
    }
    states: [
        State {
            name: "Posting"
            PropertyChanges { target: container; x: -tagButton.x + 5 }
            PropertyChanges { target: titleBar; height: 80 }
            PropertyChanges { target: tagButton; text: "OK" }
            PropertyChanges { target: tagButton; width: 28 }
            PropertyChanges { target: tagButton; height: 24 }
            PropertyChanges { target: txtEdit; focus: true }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "*"
            NumberAnimation { properties: "x,y,width,height"; easing.type: "InOutQuad" }
        }
    ]
}
