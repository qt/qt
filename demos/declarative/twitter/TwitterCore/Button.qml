import Qt 4.7

Item {
    id: container

    signal clicked

    property string text
    property bool keyUsing: false

    BorderImage {
        id: buttonImage
        source: "images/toolbutton.sci"
        width: container.width; height: container.height
    }
    BorderImage {
        id: pressed
        opacity: 0
        source: "images/toolbutton.sci"
        width: container.width; height: container.height
    }
    MouseArea {
        id: mouseRegion
        anchors.fill: buttonImage
        onClicked: { container.clicked(); }
    }
    Text {
        id: btnText
        color: if(container.keyUsing){"#DDDDDD";} else {"#FFFFFF";}
        anchors.centerIn: buttonImage; font.bold: true
        text: container.text; style: Text.Raised; styleColor: "black"
        font.pixelSize: 12
    }
    states: [
        State {
            name: "Pressed"
            when: mouseRegion.pressed == true
            PropertyChanges { target: pressed; opacity: 1 }
        },
        State {
            name: "Focused"
            when: container.focus == true
            PropertyChanges { target: btnText; color: "#FFFFFF" }
        }
    ]
    transitions: Transition {
        ColorAnimation { target: btnText; }
    }
}
