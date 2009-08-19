import Qt 4.6
Rect { border.color: "black"; color: "steelblue"; radius: 5; width: pix.width + text.width + 13; height: pix.height + 10; id: page
    property string text
    property string icon
    signal clicked
    Image { id: pix; x: 5; y:5; source: parent.icon}
    Text { id: text; text: page.text; color: "white"; x:pix.width+pix.x+3; anchors.verticalCenter: pix.verticalCenter;}
    MouseRegion{ id:mr; anchors.fill: parent; onClicked: {parent.focus = true; page.clicked()}}

    states:
        State{ name:"pressed"; when:mr.pressed
            SetProperties {target:text; x: 5}
            SetProperties {target:pix; x:text.x+text.width + 3}
        }

    transitions:
        Transition{
            NumberAnimation{ properties:"x,left"; easing:"easeInOutQuad"; duration:200 }
        }
}
