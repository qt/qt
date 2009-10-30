import Qt 4.6

Item {
    id: wrapper
    Column {
        anchors.centerIn: parent
        spacing: 20
        Row{
            spacing: 4
            Text {
                width: 100
                text: "Screen name:"
                font.pointSize: 14; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 160
                height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: nameIn
                    width: parent.width - 8
                    height: parent.height - 12
                    anchors.centerIn: parent
                    maximumLength:21
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                    Keys.forwardTo: [(tabber), (nameIn)]
                    Item {
                        id: tabber
                        //Note: it's not working yet
                        Keys.onPressed: {if(event.key == Qt.Key_Tab){print('Tab works!'); passIn.focus = true; accept(); }}
                    }
                }
            }
        }
        Row{
            spacing: 4
            Text {
                width: 100
                text: "Password:"
                font.pointSize: 14; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 160
                height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: passIn
                    width: parent.width - 8
                    height: parent.height - 12
                    anchors.centerIn: parent
                    maximumLength:21
                    echoMode: TextInput.Password
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                }
            }
        }
        Item{
            width: childrenRect.width; height:childrenRect.height;
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                x: 10
                width: 100
                height: 32
                id: login
                text: "Log in"
                onClicked: {rssModel.authName=nameIn.text; rssModel.authPass=passIn.text; rssModel.tags='my timeline';}
            }
            Button {
                x: 120
                width: 100
                height: 32
                id: guest
                text: "Guest"
                onClicked: {rssModel.authName='-'; screen.setMode(true);}
            }
        }
    }
}
