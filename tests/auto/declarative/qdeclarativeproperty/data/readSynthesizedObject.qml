import Qt 4.7

QtObject {
    property TestType test

    test: TestType {
        property int b: 19
    }
}
