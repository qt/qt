import Qt 4.7

Item {
    height: homeBar.height
    HomeTitleBar { id: homeBar; width: parent.width; height: 60;
        onUpdate: rssModel.reload()
    }
    TitleBar { id: titleBar; width: parent.width; height: 60;
        y: -80
        untaggedString: "Latest tweets from everyone"
        taggedString: "Latest tweets from "
    }
    states: [
        State {
            name: "search"; when: screen.userView
            PropertyChanges { target: titleBar; y: 0 }
            PropertyChanges { target: homeBar; y: -80 }
        }
    ]
    transitions: [
        Transition { NumberAnimation { properties: "x,y"; duration: 500; easing.type: "InOutQuad" } }
    ]
}

