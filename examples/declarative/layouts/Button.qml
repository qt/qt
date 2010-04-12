import Qt 4.7

Rectangle { border.color: "black"; color: "steelblue"; radius: 5; width: pix.width + textelement.width + 13; height: pix.height + 10; id: page
    property string text
    property string icon
    signal clicked

    Image { id: pix; x: 5; y:5; source: parent.icon}
    Text { id: textelement; text: page.text; color: "white"; x:pix.width+pix.x+3; anchors.verticalCenter: pix.verticalCenter;}
    MouseArea{ id:mr; anchors.fill: parent; onClicked: {parent.focus = true; page.clicked()}}

    states:
        State{ name:"pressed"; when:mr.pressed
            PropertyChanges {target:textelement; x: 5}
            PropertyChanges {target:pix; x:textelement.x+textelement.width + 3}
        }

    transitions:
        Transition{
            NumberAnimation { properties:"x,left"; easing.type:"InOutQuad"; duration:200 }
        }
}
