Rect { id:block
    //Note: These properties are the interface used to control the blocks
    property bool dying: false
    property bool spawning: false
    property int type: 0
    property int targetY: 0
    property int targetX: 0

    color: {if(type==0){"red";}else if(type==1){"blue";}else{"green";}}
    pen.width: 1
    pen.color: "black"
    opacity: 0
    y: targetY
    x: targetX
    y: Behavior { NumberAnimation { properties:"y"; duration: 200 } }
    opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }

    states: [

        State{ name: "SpawnState"; when: spawning == true && dying == false
            SetProperties { target: block; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            SetProperties { target: block; opacity: 0 }
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
