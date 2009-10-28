import Qt 4.6

Object {
    property var obj: nested

    property var obj2
    obj2: NestedTypeTransientErrors {
        id: nested
    }
}
