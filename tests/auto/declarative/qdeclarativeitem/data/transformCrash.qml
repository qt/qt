import Qt 4.7

Item {
    id: wrapper
    width: 200
    height: 200

    QtObject {
        id: object
    }

    Component.onCompleted: wrapper.transform = object
}
