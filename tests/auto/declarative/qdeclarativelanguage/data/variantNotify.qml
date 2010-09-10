import Qt 4.7

QtObject {
    property int notifyCount: 0

    property variant foo
    onFooChanged: notifyCount++

    Component.onCompleted: {
        foo = 1;
        foo = 1;
    }
}
