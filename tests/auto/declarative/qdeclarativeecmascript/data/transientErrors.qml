import Qt 4.6

QtObject {
    property var obj: nested

    property var obj2
    obj2: NestedTypeTransientErrors {
        id: nested
    }
}
