import Qt 4.7
import Qt.labs.particles 1.0

Item  {
    id: root
    property bool dying: false
    property int row;
    property int column;
    x: margin + column * gridSize
    y: margin + row * gridSize

    width: gridSize
    height: gridSize
    property int value : 1;

    Image {
        id: img
        anchors.fill: parent
        source: "pics/cookie.png"
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 100 } }
        Text {
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: value
        }
    }


    Particles { id: particles
        width:1; height:1; anchors.centerIn: parent;
        emissionRate: 0;
        lifeSpan: 700; lifeSpanDeviation: 600;
        angle: 0; angleDeviation: 360;
        velocity: 100; velocityDeviation:30;
        source: "pics/yellowStar.png";
    }

    states: [
        State{ name: "AliveState"; when: dying == false
            PropertyChanges { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 0 }
        }
    ]
}
