import Qt 4.6

Object {
    property var nested
    nested: NestedObject { }
    property var nested2: nested.nested
}

