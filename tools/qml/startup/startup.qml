import Qt 4.7

Rectangle {
    id: treatsApp
    width: 800
    height: 480
    color: "darkgrey"
    Component.onCompleted: treatsApp.state = "part1"
    signal animationFinished

    Logo {
        id: logo
        x: 165
        y: 35
        rotation: -15
        scale: 0.6
        opacity: 0
        onAnimationFinished: treatsApp.animationFinished();
    }

    states: [
        State {
            name: "part1"
            PropertyChanges {
                target: logo
                scale: 0.8
                opacity: 1
                rotation: 0
            }
            PropertyChanges {
                target: treatsApp
                color: "black"
            }
            PropertyChanges {
                target: logo
                y: 10
            }
            PropertyChanges {
                target: quickblur
                x: logo.x + 145
            }
            PropertyChanges {
                target: blurText
                opacity: 0
            }
            PropertyChanges {
                target: quickregular
                opacity: 1
            }
            PropertyChanges {
                target: star
                x: -7
                y: -37
            }
        }
    ]


    Item {
        id: quickblur
        x: 800//325
        y: 344
        Image {
            id: blurText
            source: "quick-blur.png"
        }
        Image {
            id: quickregular
            x: -1
            y: 0
            opacity: 0
            source: "quick-regular.png"
        }
        Image {
            id: star
            x: -1
            y: 0
            opacity: 0
            source: "white-star.png"
            smooth: true
            NumberAnimation on rotation {
                from: 0
                to: 360
                loops: NumberAnimation.Infinite
                running: true
                duration: 2000                
            }   
        }
    }

    transitions: [
        Transition {
            ParallelAnimation {
                NumberAnimation { target: logo; property: "opacity"; duration: 500 }
                NumberAnimation { target: logo; property: "scale"; duration: 4000; }
                NumberAnimation { target: logo; property: "rotation"; duration: 2000; easing.type: "OutBack"}
                ColorAnimation { duration: 3000}
                SequentialAnimation {
                    PauseAnimation { duration: 1000 }
                    ScriptAction { script: logo.logoState = "showBlueprint" }
                    PauseAnimation { duration: 800 }
                    ScriptAction { script: logo.logoState = "finale" }
                    PauseAnimation { duration: 800 }
                    ParallelAnimation {
                        NumberAnimation { target: quickblur; property: "x"; duration: 200;}
                        SequentialAnimation {
                            PauseAnimation { duration: 200}
                            ParallelAnimation {
                                NumberAnimation { target: blurText; property: "opacity"; duration: 300;}
                                NumberAnimation { target: quickregular; property: "opacity"; duration: 300;}
                            }
                            NumberAnimation { target: star; property: "opacity"; from: 0; to: 1; duration: 500 }
                            PauseAnimation { duration: 200 }
                            NumberAnimation { target: star; property: "opacity"; from: 1; to: 0; duration: 500 }
                        }
                        SequentialAnimation {
                            PauseAnimation { duration: 150}
                            NumberAnimation { target: logo; property: "y"; duration: 300; easing.type: "OutBounce" }
                        }
                    }
                }
            }
        }
    ]   

} // treatsApp
