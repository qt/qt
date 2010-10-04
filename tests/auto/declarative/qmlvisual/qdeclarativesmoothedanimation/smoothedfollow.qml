import QtQuick 1.0

Rectangle {
    width: 800; height: 720; color: "gray"

    Rectangle {
        id: rect
        width: 50; height: 20; y: 30; color: "black"
        SequentialAnimation on x {
            loops: Animation.Infinite
            NumberAnimation { from: 50; to: 700; duration: 1000 }
            NumberAnimation { from: 700; to: 50; duration: 1000 }
        }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 60; color: "red"
        Behavior on x { SmoothedAnimation { velocity: 400 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 90; color: "yellow"
        Behavior on x { SmoothedAnimation { velocity: 300; reversingMode: SmoothedAnimation.Immediate } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 120; color: "green"
        Behavior on x { SmoothedAnimation { velocity: 200; reversingMode: SmoothedAnimation.Sync } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 150; color: "purple"
        Behavior on x { SmoothedAnimation { velocity: 200; maximumEasingTime: 100 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect.x; y: 180; color: "blue"
        Behavior on x { SmoothedAnimation { velocity: -1; duration: 300 } }
    }

    //rect2 has jerky movement, but the rects following it should be smooth
    Rectangle {
        id: rect2
        property int dir: 1
        width: 50; height: 20; x:50; y: 240; color: "black"
        function advance(){
            if(x >= 700)
                dir = -1;
            if(x <= 50)
                dir = 1;
            x += 130.0 * dir;
        }
    }
    Timer{
        interval: 200
        running: true
        repeat: true
        onTriggered: rect2.advance();
    }

    Rectangle {
        width: 50; height: 20; x: rect2.x; y: 270; color: "red"
        Behavior on x { SmoothedAnimation { velocity: 400 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect2.x; y: 300; color: "yellow"
        Behavior on x { SmoothedAnimation { velocity: 300; reversingMode: SmoothedAnimation.Immediate } }
    }

    Rectangle {
        width: 50; height: 20; x: rect2.x; y: 330; color: "green"
        Behavior on x { SmoothedAnimation { velocity: 200; reversingMode: SmoothedAnimation.Sync } }
    }

    Rectangle {
        width: 50; height: 20; x: rect2.x; y: 360; color: "purple"
        Behavior on x { SmoothedAnimation { velocity: 200; maximumEasingTime: 100 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect2.x; y: 390; color: "blue"
        Behavior on x { SmoothedAnimation { velocity: -1; duration: 300 } }
    }

    //rect3 just jumps , but the rects following it should be smooth
    Rectangle {
        id: rect3
        width: 50; height: 20; x:50; y: 480; color: "black"
        function advance(){
            if(x == 50)
                x = 700;
            else
                x = 50;
        }
    }
    Timer{
        interval: 1000
        running: true
        repeat: true
        onTriggered: rect3.advance();
    }

    Rectangle {
        width: 50; height: 20; x: rect3.x; y: 510; color: "red"
        Behavior on x { SmoothedAnimation { velocity: 400 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect3.x; y: 540; color: "yellow"
        Behavior on x { SmoothedAnimation { velocity: 300; reversingMode: SmoothedAnimation.Immediate } }
    }

    Rectangle {
        width: 50; height: 20; x: rect3.x; y: 570; color: "green"
        Behavior on x { SmoothedAnimation { velocity: 200; reversingMode: SmoothedAnimation.Sync } }
    }

    Rectangle {
        width: 50; height: 20; x: rect3.x; y: 600; color: "purple"
        Behavior on x { SmoothedAnimation { velocity: 200; maximumEasingTime: 100 } }
    }

    Rectangle {
        width: 50; height: 20; x: rect3.x; y: 630; color: "blue"
        Behavior on x { SmoothedAnimation { velocity: -1; duration: 300 } }
    }
}
