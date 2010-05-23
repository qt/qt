import Qt 4.7

Rectangle {
    width: 300; height: 300;
    Rectangle {
        objectName: "theRect"
        color: "red"
        width: 60; height: 60;
        x: 100; y: 100;
        SmoothedFollow on x { id: animX; objectName: "animX"; to: 200; enabled: true; duration: 200 }
        SmoothedFollow on y { id: animY; objectName: "animY"; to: 200; enabled: false; duration: 200 }
    }
}
