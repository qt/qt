import Qt 4.6

Rect { id:block
    //Note: These properties are the interface used to control the blocks
    property bool dying: false
    property bool spawned: false
    property bool selected: false
    property int type: 0
    property int targetY: 0
    property int targetX: 0

    color: {if(type==0){"red";}else if(type==1){"blue";}else{"green";}}
    pen.width: 1
    pen.color: "black"
    opacity: 0
    y: targetY
    x: targetX

    MouseRegion {
        id: gameMR; anchors.fill: parent
        onClicked: handleClick(Math.floor(parent.x/width), Math.floor(parent.y/height));
    }

    states: [

        State{ name: "AliveState"; when: spawned == true && dying == false
            SetProperties { target: block; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            SetProperties { target: block; opacity: 0 }
        }
    ]
}
