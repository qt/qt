import Qt 4.6

QtObject {
    property QtObject object
    property alias aliasedObject: target.object

    object: QtObject {
        id: target

        property QtObject object
        object: QtObject {}
    }
}

