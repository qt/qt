import Qt 4.6

Object {
    property Object object
    property alias aliasedObject: target.object

    object: Object {
        id: target

        property Object object
        object: Object {}
    }
}

