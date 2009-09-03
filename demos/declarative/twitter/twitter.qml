import Qt 4.6
import "content" as Twitter
import "../flickr/common" as Common
import "../flickr/mobile" as Mobile

Item {
    id: Screen; width: 320; height: 480
    property bool userView : false 
    function setMode(m){
        Screen.userView = m;
        if(m == false){ 
            RssModel.tags='my timeline';
            RssModel.reload();
            ToolBar.button2Label = "View others";
        } else {
            ToolBar.button2Label = "Return home";
        }
    }
    //Workaround for bug 260266
    Timer{ interval: 1; running: false; repeat: false; onTriggered: reallySetUser(); id:hack }
    Script{
        var tmpStr;
        function setUser(str){hack.running = true; tmpStr = str}
        function reallySetUser(){RssModel.tags = tmpStr;}
    }

    Rectangle {
        id: Background
        anchors.fill: parent; color: "#343434";

        Image { source: "mobile/images/stripes.png"; fillMode: "Tile"; anchors.fill: parent; opacity: 0.3 }

        Twitter.RssModel { id: RssModel }
        Common.Loading { anchors.centerIn: parent; visible: RssModel.status==XmlListModel.Loading && state!='unauthed'}
        Text { 
            width: 180
            text: "Could not access twitter using this screen name and password pair."; 
            color: "white"; color: "#cccccc"; style: "Raised"; styleColor: "black"; wrap: true
            visible: RssModel.status==XmlListModel.Error; anchors.centerIn: parent
        }

        Item {
            id: Views
            x: 2; width: parent.width - 4
            y:60 //Below the title bars
            height: 320

            Twitter.AuthView{
                id: authView
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: parent.height-60; 
                x: -(Screen.width * 1.5)
            }

            Twitter.FatDelegate { id: FatDelegate }
            ListView {
                id: MainView; model: RssModel.model; delegate: FatDelegate;
                width: parent.width; height: parent.height; x: 0; cacheBuffer: 100;
            }
        }

        Twitter.MultiTitleBar { id: TitleBar; width: parent.width }
        Mobile.ToolBar { id: ToolBar; height: 40; 
            //anchors.bottom: parent.bottom; 
            //TODO: Use anchor changes instead of hard coding
            y: 440
            width: parent.width; opacity: 0.9 
            button1Label: "Update"
            button2Label: "View others"
            onButton2Clicked: 
            {
                if(Screen.userView == true){
                    Screen.setMode(false);
                }else{
                    Screen.setMode(true);
                }
            }
        }

        states: [
            State {
                name: "unauthed"; when: RssModel.authName==""
                PropertyChanges { target: authView; x: 0 }
                PropertyChanges { target: MainView; x: -(parent.width * 1.5) }
                PropertyChanges { target: TitleBar; y: -80 }
                PropertyChanges { target: ToolBar; y: Screen.height + 80 }
            }
        ]
        transitions: [
            Transition { NumberAnimation { properties: "x,y"; duration: 500; easing: "easeInOutQuad" } }
        ]
    }
}
