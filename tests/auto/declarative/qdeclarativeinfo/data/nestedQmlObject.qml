import Qt 4.6

QtObject {
    property variant nested
    nested: NestedObject { }
    property variant nested2: nested.nested
}

