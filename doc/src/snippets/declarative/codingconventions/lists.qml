import Qt 4.7

Item {
    Item {
//! [0]
states: [
    State {
        name: "open"
        PropertyChanges { target: container; width: 200 }
    }
]
//! [0]
    }
    Item {
//! [1]
states: State {
    name: "open"
    PropertyChanges { target: container; width: 200 }
}
//! [1]
    }
}
