import Qt 4.7

QtObject {
    id: root
    property QtObject object: QtObject {
        id: nested
        property QtObject nestedObject
    }

    Component.onCompleted: {
        nested.nestedObject = root;
    }
}
