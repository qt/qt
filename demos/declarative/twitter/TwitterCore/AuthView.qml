import Qt 4.7

Item {
    id: wrapper
    Column {
        anchors.centerIn: parent
        spacing: 20
        Column{
            spacing: 4
            Text {
                text: "Screen name:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 220
                height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: nameIn
                    width: parent.width - 8
                    anchors.centerIn: parent
                    maximumLength:21
                    font.pixelSize: 16;
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                    KeyNavigation.tab: passIn
                    KeyNavigation.backtab: guest
                    focus: true
                }
            }
        }
        Column{
            spacing: 4
            Text {
                text: "Password:"
                font.pixelSize: 16; font.bold: true; color: "white"; style: Text.Raised; styleColor: "black"
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 220
               height: 28
                BorderImage { source: "images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: passIn
                    width: parent.width - 8
                    anchors.centerIn: parent
                    maximumLength:21
                    echoMode: TextInput.Password
                    font.pixelSize: 16;
                    font.bold: true
                    color: "#151515"; selectionColor: "green"
                    KeyNavigation.tab: login
                    KeyNavigation.backtab: nameIn
                }
            }
        }
        Row{
            spacing: 10
            Button {
                width: 100
                height: 32
                id: login
                keyUsing: true;
                function doLogin(){
                    rssModel.authName=nameIn.text;
                    rssModel.authPass=passIn.text;
                    rssModel.tags='my timeline';
                    screen.focus = true;
                }
                text: "Log in"
                KeyNavigation.right: guest
                KeyNavigation.tab: guest
                KeyNavigation.backtab: passIn
                Keys.onReturnPressed: login.doLogin();
                Keys.onSelectPressed: login.doLogin();
                Keys.onSpacePressed: login.doLogin();
                onClicked: login.doLogin();
            }
            Button {
                width: 100
                height: 32
                id: guest
                keyUsing: true;
                function doGuest()
                {
                    rssModel.authName='-';
                    screen.focus = true;
                    screen.setMode(true);
                }
                text: "Guest"
                KeyNavigation.left: login
                KeyNavigation.tab: nameIn
                KeyNavigation.backtab: login
                Keys.onReturnPressed: guest.doGuest();
                Keys.onSelectPressed: guest.doGuest();
                Keys.onSpacePressed: guest.doGuest();
                onClicked: guest.doGuest();
            }
        }
    }
}
