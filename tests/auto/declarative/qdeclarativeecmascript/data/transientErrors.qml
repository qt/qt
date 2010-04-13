import Qt 4.6

QtObject {
    property variant obj: nested

    property variant obj2
    obj2: NestedTypeTransientErrors {
        id: nested
    }
}
