import Qt 4.6

QtObject {
    property var nested
    nested: NestedObject { }
    property var nested2: nested.nested
}

