Item { id:block
    property bool dying: false
    property bool spawning: false
    property int type: 0
    property int targetX: 0
    property int targetY: 0

    x: targetX
    y: Follow { source: targetY; spring: 1.2; damping: 0.1 }
    //y: Behavior { NumberAnimation { properties:"y"; duration: 200 } }

    Image { id: img
        source: {if(type==0){"pics/redStone.png";}else if(type==1){"pics/blueStone.png";}else{"pics/greenStone.png";}}
        opacity: 0
        opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }
        anchors.fill: parent
    }
    Particles { id: particles
        width:1; height:1; anchors.centeredIn: parent; opacity: 0
        lifeSpan: 100000; source: "pics/star.png"; count:1; streamIn: false
        angle: 0; angleDeviation: 360; velocity: 100; velocityDeviation:30
    }
    states: [

        State{ name: "SpawnState"; when: spawning == true && dying == false
            SetProperties { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            SetProperties { target: particles; count: 50 }
            SetProperties { target: particles; opacity: 1 }
            SetProperties { target: img; opacity: 0 }
        }
    ]
//    transitions: [
//        Transition {
//            fromState: "SpawnState"
//            NumberAnimation { properties: "opacity"; duration: 200 }
//        },
//        Transition {
//            toState: "DeathState"
//            SequentialAnimation {
//                NumberAnimation { properties: "opacity"; duration: 200 }
//                //TODO: Warning about following line, if it works
//                //RunScriptAction { script: page.destroy() }
//            }
//        }
//    ]
}
