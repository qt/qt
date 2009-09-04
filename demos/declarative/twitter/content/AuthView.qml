import Qt 4.6
import "../../flickr/common"
import "../../flickr/mobile" 

Item {
    id: wrapper
    Column {
        width: childrenRect.width; height:childrenRect.height; anchors.centerIn: parent
        spacing: 20
        Row{
            width: childrenRect.width; height:childrenRect.height;
            spacing: 4
            Text { 
                width: 100
                text: "Screen name:"
                font.pointSize: 10; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 160
                height: 28
                BorderImage { source: "../../flickr/mobile/images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: nameIn
                    width: parent.width - 8
                    height: parent.height - 12
                    anchors.centerIn: parent
                    maximumLength:21
                    font.bold: true
                    color: "#151515"; highlightColor: "green"
                    KeyProxy {
                        id: Proxy
                        targets: [(tabber), (nameIn)]
                        focus: true
                    }
                    Item { 
                        id: tabber
                        //Note: it's not working yet
                        Keys.onPressed: {if(event.key == Qt.Key_Tab){print('Tab works!'); passIn.focus = true; accept(); }}
                    }
                }
            }
        }
        Row{
            width: childrenRect.width; height:childrenRect.height;
            spacing: 4
            Text { 
                width: 100
                text: "Password:"
                font.pointSize: 10; font.bold: true; color: "white"; style: "Raised"; styleColor: "black"
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Qt.AlignRight
            }
            Item {
                width: 160
                height: 28
                BorderImage { source: "../../flickr/mobile/images/lineedit.sci"; anchors.fill: parent }
                TextInput{
                    id: passIn
                    width: parent.width - 8
                    height: parent.height - 12
                    anchors.centerIn: parent
                    maximumLength:21
                    echoMode: 2
                    font.bold: true
                    color: "#151515"; highlightColor: "green"
                }
            }
        }
        Item{
            width: childrenRect.width; anchors.horizontalCenter: parent.horizontalCenter
            height: childrenRect.height
            Button {
                x: 10
                width: 100
                height: 32
                id: login
                text: "Log in"
                onClicked: {RssModel.authName=nameIn.text; RssModel.authPass=passIn.text; RssModel.tags='my timeline';}
            }
            Button {
                x: 120
                width: 100
                height: 32
                id: guest
                text: "Guest"
                onClicked: {RssModel.authName='-'; Screen.setMode(true);}
            }
        }
    }
}
