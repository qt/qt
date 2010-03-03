import Qt 4.6

QtObject {
    property TestType test

    test: TestType {
        property int b: 19
    }
}
