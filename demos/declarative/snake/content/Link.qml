import Qt 4.7
import Qt.labs.particles 1.0

Item { id:link
    property bool dying: false
    property bool spawned: false
    property int type: 0
    property int row: 0
    property int column: 0
    property int rotation;

    width: 40;
    height: 40

    x: margin - 3 + gridSize * column
    y: margin - 3 + gridSize * row
    Behavior on x { NumberAnimation { duration: spawned ? heartbeatInterval : 0} }
    Behavior on y { NumberAnimation { duration: spawned ? heartbeatInterval : 0 } }


    Item {
        id: img
        anchors.fill: parent
        Image {
            source: {
                if(type == 1) {
                    "pics/blueStone.png";
                } else if (type == 2) {
                    "pics/head.png";
                } else {
                    "pics/redStone.png";
                }
            }

            transform: Rotation {
                id: actualImageRotation
                origin.x: width/2; origin.y: height/2;
                angle: rotation * 90
                Behavior on angle { NumberAnimation { duration: spawned ? 200 : 0} }
            }
        }

        Image {
            source: "pics/stoneShadow.png"
        }

        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }


    Particles { id: particles
        width:1; height:1; anchors.centerIn: parent;
        emissionRate: 0;
        lifeSpan: 700; lifeSpanDeviation: 600;
        angle: 0; angleDeviation: 360;
        velocity: 100; velocityDeviation:30;
        source: {
            if(type == 1){
                "pics/blueStar.png";
            } else {
                "pics/redStar.png";
            }
        }
    }

    states: [
        State{ name: "AliveState"; when: spawned == true && dying == false
            PropertyChanges { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 0 }
        }
    ]
}
