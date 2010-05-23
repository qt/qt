import Qt 4.7

Rectangle {
    id: myItem

    states : State {
        PropertyChanges {
            target: myItem
            children: Item { id: newItem }
        }
    }
}
