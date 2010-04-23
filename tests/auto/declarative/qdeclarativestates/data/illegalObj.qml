import Qt 4.6

Rectangle {
    id: myItem

    states : State {
        PropertyChanges {
            target: myItem
            children: Item { id: newItem }
        }
    }
}
