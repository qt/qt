import "content"

Item {
    id: MainWindow; width: 800; height: 480

    properties: Property { name: "minimized"; value: false }

    Item {
        id: Background

        Image { source: "content/pics/background.png"; opaque: true }

        Rect {
            id: Menu; x: -150; width: 150; height: 480; color: "#232323"

            Text {
                id: IconText; x: 40; y: 110
                font.bold: true; font.size: 9; text: "Nokia Devices"
                color: "white"; style: Raised; styleColor: "black"
            }

            Image {
                source: "content/pics/shadow-right-screen.png"
                x: 150; height: 480; tile: true
            }
        }

        PhonesPathView { id: PhonesPathView; model: PhonesModel; y: 60; width: 800; height: 360 }
        PhoneInfoContainer { id: PhoneInfoContainer; width: 750; x: 25; y: 500; height: 440 }

        MediaButton {
            id: CloseButton; x: 680; y: 440; text: "Close"
            onClicked: { MainWindow.minimized = true }
        }

        states: [
            State {
                name: "Minimized"
                when: MainWindow.minimized == true
                SetProperties { target: Background; x: 75 }
                SetProperties { target: Menu; x: -75 }
            }
        ]

        transitions: [
            Transition {
                fromState: "*"; toState: "*"
                NumericAnimation { properties: "x,y,size"; duration: 500; easing: "easeInOutQuad" }
            }
        ]
    }

    Text {
        id: CategoryText; x: 300; y: 15; text: "Nokia Devices"
        font.size: 22; color: "white"; style: Raised; styleColor: "black"
    }

    Item { id: Stack; x: 45; y: 50 }
}
