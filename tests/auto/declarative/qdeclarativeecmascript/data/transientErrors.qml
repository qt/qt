import Qt 4.7

QtObject {
    property variant obj: nested

    property variant obj2
    obj2: NestedTypeTransientErrors {
        id: nested
    }
}
