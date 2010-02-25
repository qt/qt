import Qt 4.6

Rectangle {
    width: 640
    height: 480
    Text {
        id: theText
        width: 50
        wrap: true
        text: "a text string that is longer than 50 pixels"
    }

    states: State {
        name: "state1"
        PropertyChanges {
            target: theText
            width: undefined
        }
    }
}
