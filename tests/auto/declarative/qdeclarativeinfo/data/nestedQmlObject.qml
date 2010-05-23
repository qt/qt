import Qt 4.7

QtObject {
    property variant nested
    nested: NestedObject { }
    property variant nested2: nested.nested
}

