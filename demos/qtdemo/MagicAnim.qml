import Qt 4.7

//Emulates the in animation of the menu elements
SequentialAnimation{
    id: main;
    property Item target
    property int from: 0
    property int to: 100
    property int duration: 1000
    property string properties: "y"
    PauseAnimation { duration: main.duration*0.20 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.from; to: main.to + 40; duration: main.duration*0.30 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.to + 40; to: main.to; duration: main.duration*0.10 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.to; to: main.to + 20; duration: main.duration*0.10 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.to + 20; to: main.to; duration: main.duration*0.10 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.to; to: main.to + 8; duration: main.duration*0.10 }
    NumberAnimation { target: main.target; properties: main.properties; from: main.to + 8; to: main.to; duration: main.duration*0.10 }
}

