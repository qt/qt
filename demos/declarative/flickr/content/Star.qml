Item {
    id: Container
    width: 24
    height: 24

    property string rating
    property string on

    signal clicked

    Image {
        id: Image
        source: "pics/ghns_star.png"
        x: 6
        y: 7
        opacity: 0.4
        scale: 0.5
    }
    MouseRegion {
        anchors.fill: Container
        onClicked: { Container.clicked() }
    }
    states: [
        State {
            name: "on"
            when: Container.on == true
            SetProperties {
                target: Image
                opacity: 1
                scale: 1
                x: 1
                y: 0
            }
        }
    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity,scale,x,y"
                easing: "easeOutBounce"
            }
        }
    ]
}
