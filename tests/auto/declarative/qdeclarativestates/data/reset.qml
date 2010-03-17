import Qt 4.6

Rectangle {
    width: 640
    height: 480
    Text {
        id: theText
        width: 40
        wrap: true
        text: "a text string that is longer than 40 pixels"
    }

    states: State {
        name: "state1"
        PropertyChanges {
            target: theText
            width: undefined
        }
    }
}
