Item { id:block
    //Note: These properties are the interface used to control the blocks
    property bool dying: false
    property bool spawned: false
    property int type: 0
    property int targetY: 0
    property int targetX: 0

    AnimatedImage {
        source: if(type == 0) {
                    "pics/gnome/redStone.gif"; 
                } else if (type == 1) { 
                    "pics/gnome/blueStone.gif"; 
                } else {
                    "pics/gnome/greenStone.gif"; 
                }
    }
    opacity: 0
    y: targetY
    x: targetX
    y: Behavior { NumberAnimation { properties:"y"; duration: 200 } }
    x: Behavior { NumberAnimation { properties:"x"; duration: 200 } }
    opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }

    states: [

        State{ name: "AliveState"; when: spawned == true && dying == false
            SetProperties { target: block; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            SetProperties { target: block; opacity: 0 }
        }
    ]
}
