import Qt 4.6

Item { id:block
    //Note: These properties are the interface used to control the blocks
    property bool dying: false
    property bool spawned: false
    property bool selected: false
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
        paused: !selected
        paused: Behavior { to: true; from: false;
            NumberAnimation { properties:"currentFrame"; to:0; duration: 200}
        }
    }
    opacity: 0
    y: targetY
    x: targetX
    y: Behavior { NumberAnimation { properties:"y"; duration: 200 } }
    x: Behavior { NumberAnimation { properties:"x"; duration: 200 } }
    opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }
    MouseRegion {
        id: gameMR; anchors.fill: parent
        onClicked: handleClick(Math.floor(parent.x/width), Math.floor(parent.y/height));
        onEntered: handleHover(Math.floor(parent.x/width), Math.floor(parent.y/height));
        onExited: handleHover(Math.floor(parent.x/width), Math.floor(parent.y/height));
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
