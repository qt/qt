Item { id:block
    property bool dying: false
    property bool spawning: false
    property int type: 0
    property int targetX: 0
    property int targetY: 0

    x: Follow { source: targetX; spring: 1.2; damping: 0.1 }
    y: Follow { source: targetY; spring: 1.2; damping: 0.1 }

    Image { id: img
        source: {
            if(type == 0){
                "pics/redStone.png";
            } else if(type == 1) {
                "pics/blueStone.png";
            } else {
                "pics/greenStone.png";
            }
        }
        opacity: 0
        opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }
        anchors.fill: parent
    }
    Particles { id: particles
        width:1; height:1; anchors.centeredIn: parent; opacity: 0
        lifeSpan: 1000000000; count:0; streamIn: false
        angle: 0; angleDeviation: 360; velocity: 100; velocityDeviation:30
        source: {
            if(type == 0){
                "pics/redStar.png";
            } else if (type == 1) {
                "pics/blueStar.png";
            } else {
                "pics/greenStar.png";
            }
        }
    }
    states: [

        State{ name: "AliveState"; when: spawning == true && dying == false
            SetProperties { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            SetProperties { target: particles; count: 50 }
            SetProperties { target: particles; opacity: 1 }
            SetProperties { target: img; opacity: 0 }
        }
    ]
}
