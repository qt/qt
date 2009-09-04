import Qt 4.6
import "../../flickr/mobile"

Item {
    height: HomeBar.height
    HomeTitleBar { id: HomeBar; width: parent.width; height: 60; 
        onUpdate: RssModel.reload()
    }
    TitleBar { id: TitleBar; width: parent.width; height: 60;
        y: -80
        untaggedString: "Latest tweets from everyone"
        taggedString: "Latest tweets from "
    }
    states: [
        State {
            name: "search"; when: Screen.userView
            PropertyChanges { target: TitleBar; y: 0 }
            PropertyChanges { target: HomeBar; y: -80 }
        }
    ]
    transitions: [
        Transition { NumberAnimation { properties: "x,y"; duration: 500; easing: "easeInOutQuad" } }
    ]
}

