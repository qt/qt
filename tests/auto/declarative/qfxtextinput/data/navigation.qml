import Qt 4.6

Rectangle {
    property var myInput: Input

    width: 800; height: 600; color: "blue"

    Item { 
        id: FirstItem;
        KeyNavigation.right: Input
    }

    TextInput { id: Input; focus: true
        KeyNavigation.left: FirstItem
        KeyNavigation.right: LastItem
        KeyNavigation.up: FirstItem
        KeyNavigation.down: LastItem
    }
    Item {
        id: LastItem 
        KeyNavigation.left: Input
    }
}
