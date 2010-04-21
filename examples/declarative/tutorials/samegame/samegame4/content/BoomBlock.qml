import Qt 4.7
import Qt.labs.particles 1.0

Item {
    id: block

    property int type: 0
    property bool dying: false

    //![1]
    property bool spawned: false
    property int targetX: 0
    property int targetY: 0

    SpringFollow on x { to: targetX; spring: 2; damping: 0.2; enabled: spawned }
    SpringFollow on y { to: targetY; spring: 2; damping: 0.2 }
    //![1]

    //![2]
    Image { 
        id: img

        anchors.fill: parent
        source: {
            if (type == 0)
                return "../../shared/pics/redStone.png";
            else if (type == 1) 
                return "../../shared/pics/blueStone.png";
            else
                return "../../shared/pics/greenStone.png";
        }
        opacity: 0

        Behavior on opacity { 
            NumberAnimation { properties:"opacity"; duration: 200 }
        }
    }
    //![2]

    //![3]
    Particles {
        id: particles

        width: 1; height: 1 
        anchors.centerIn: parent

        emissionRate: 0
        lifeSpan: 700; lifeSpanDeviation: 600
        angle: 0; angleDeviation: 360;
        velocity: 100; velocityDeviation: 30
        source: {
            if (type == 0)
                return "../../shared/pics/redStar.png";
            else if (type == 1) 
                return "../../shared/pics/blueStar.png";
            else
                return "../../shared/pics/greenStar.png";
        }
    }
    //![3]

    //![4]
    states: [
        State {
            name: "AliveState"
            when: spawned == true && dying == false
            PropertyChanges { target: img; opacity: 1 }
        },

        State {
            name: "DeathState"
            when: dying == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 0 }
            StateChangeScript { script: block.destroy(1000); }
        }
    ]
    //![4]
}
