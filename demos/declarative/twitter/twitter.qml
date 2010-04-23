import Qt 4.7
import "TwitterCore" 1.0 as Twitter

Item {
    id: screen; width: 320; height: 480
    property bool userView : false
    property variant tmpStr
    function setMode(m){
        screen.userView = m;
        if(m == false){
            rssModel.tags='my timeline';
            rssModel.reload();
            toolBar.button2Label = "View others";
        } else {
            toolBar.button2Label = "Return home";
        }
    }
    function setUser(str){hack.running = true; tmpStr = str}
    function reallySetUser(){rssModel.tags = tmpStr;}

    //Workaround for bug 260266
    Timer{ interval: 1; running: false; repeat: false; onTriggered: screen.reallySetUser(); id:hack }

    //TODO: better way to return to the auth screen
    Keys.onEscapePressed: rssModel.authName=''
    Rectangle {
        id: background
        anchors.fill: parent; color: "#343434";

        Image { source: "TwitterCore/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }

        Twitter.RssModel { id: rssModel }
        Twitter.Loading { anchors.centerIn: parent; visible: rssModel.status==XmlListModel.Loading && state!='unauthed'}
        Text {
            width: 180
            text: "Could not access twitter using this screen name and password pair.";
            color: "#cccccc"; style: Text.Raised; styleColor: "black"; wrapMode: Text.WordWrap
            visible: rssModel.status==XmlListModel.Error; anchors.centerIn: parent
        }

        Item {
            id: views
            x: 2; width: parent.width - 4
            y:60 //Below the title bars
            height: 380

            Twitter.AuthView{
                id: authView
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: parent.height-60;
                x: -(screen.width * 1.5)
            }

            Twitter.FatDelegate { id: fatDelegate }
            ListView {
                id: mainView; model: rssModel.model; delegate: fatDelegate;
                width: parent.width; height: parent.height; x: 0; cacheBuffer: 100;
            }
        }

        Twitter.MultiTitleBar { id: titleBar; width: parent.width }
        Twitter.ToolBar { id: toolBar; height: 40;
            //anchors.bottom: parent.bottom;
            //TODO: Use anchor changes instead of hard coding
            y: screen.height - 40
            width: parent.width; opacity: 0.9
            button1Label: "Update"
            button2Label: "View others"
            onButton1Clicked: rssModel.reload();
            onButton2Clicked:
            {
                if(screen.userView == true){
                    screen.setMode(false);
                }else{
                    rssModel.tags='';
                    screen.setMode(true);
                }
            }
        }

        states: [
            State {
                name: "unauthed"; when: rssModel.authName==""
                PropertyChanges { target: authView; x: 0 }
                PropertyChanges { target: mainView; x: -(parent.width * 1.5) }
                PropertyChanges { target: titleBar; y: -80 }
                PropertyChanges { target: toolBar; y: screen.height }
            }
        ]
        transitions: [
            Transition { NumberAnimation { properties: "x,y"; duration: 500; easing.type: "InOutQuad" } }
        ]
    }
}
