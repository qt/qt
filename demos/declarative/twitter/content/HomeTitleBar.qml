import Qt 4.6
import "../../flickr/mobile"
import "../../flickr/common"

Item {
    id: titleBar

    signal update()
    onYChanged: state="" //When switching titlebars

    BorderImage { source: "../../flickr/mobile/images/titlebar.sci"; width: parent.width; height: parent.height + 14; y: -7 }
    Item {
        id: Container
        width: (parent.width * 2) - 55 ; height: parent.height

        Script {
            function accept() {
                if(RssModel.authName == '' || RssModel.authPass == '')
                    return false;//Can't login like that

                var postData = "status=" + Editor.text;
                var postman = new XMLHttpRequest();
                postman.open("POST", "http://twitter.com/statuses/update.xml", true, RssModel.authName,  RssModel.authPass);
                postman.onreadystatechange = function() { 
                    if (postman.readyState == postman.DONE) {
                        titleBar.update();
                    }
                }
                postman.send(postData);

                Editor.text = ""
                titleBar.state = ""
            }
        }

        Rectangle {
            id: WhiteRect; x: 6; width: 50; height: 50; color: "white"; smooth: true
            anchors.verticalCenter: parent.verticalCenter

            UserModel { user: RssModel.authName; id: userModel }
            Component { id: imgDelegate; 
                Item { id: Wrapper
                    Loading { width:48; height:48; visible: realImage.status != 1 } 
                    Image { source: image; width:48; height:48; id: realImage } 
                }
            } 
            ListView { model: userModel.model; x:1; y:1; delegate: imgDelegate }
        }

        Text {
            id: CategoryText
            anchors.left: parent.left; anchors.right: TagButton.left
            anchors.leftMargin: 58; anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            elide: "ElideLeft"
            text: "Timeline for " + RssModel.authName
            font.pointSize: 10; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
        }

        Button {
            id: TagButton; x: titleBar.width - 90; width: 85; height: 32; text: "New Post..."
            anchors.verticalCenter: parent.verticalCenter;
            onClicked: if (titleBar.state == "Posting") accept(); else titleBar.state = "Posting"
        }

        Text {
            id: charsLeftText; anchors.horizontalCenter: TagButton.horizontalCenter;
            anchors.top: TagButton.bottom; anchors.topMargin: 2
            text: {140 - Editor.text.length;} visible: titleBar.state == "Posting"
            font.pointSize: 10; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
        }
        Item {
            id: txtEdit;
            anchors.left: TagButton.right; anchors.leftMargin: 5; y: 4
            anchors.right: parent.right; anchors.rightMargin: 40; height: parent.height - 9
            BorderImage { source: "../../flickr/mobile/images/lineedit.sci"; anchors.fill: parent }

            Binding {//TODO: Can this be a function, which also resets the cursor? And flashes?
                when: Editor.text.length > 140
                target: Editor
                property: "text"
                value: Editor.text.slice(0,140)
            }
            TextEdit {
                id: Editor
                anchors.left: parent.left; 
                anchors.leftMargin: 8;
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4;
                cursorVisible: true; font.bold: true
                width: parent.width - 12
                height: parent.height - 8
                font.pointSize: 10
                wrap:true
                color: "#151515"; selectionColor: "green"
            }
            Keys.forwardTo: [(ReturnKey), (Editor)]
            Item {
                id: ReturnKey
                Keys.onReturnPressed: accept()
                Keys.onEscapePressed: titleBar.state = ""
            }
        }
    }
    states: [
        State {
            name: "Posting"
            PropertyChanges { target: Container; x: -TagButton.x + 5 }
            PropertyChanges { target: titleBar; height: 80 }
            PropertyChanges { target: TagButton; text: "OK" }
            PropertyChanges { target: TagButton; width: 28 }
            PropertyChanges { target: TagButton; height: 24 }
            PropertyChanges { target: txtEdit; focus: true }
        }
    ]
    transitions: [
        Transition {
            from: "*"; to: "*"
            NumberAnimation { properties: "x,y,width,height"; easing: "easeInOutQuad" }
        }
    ]
}
