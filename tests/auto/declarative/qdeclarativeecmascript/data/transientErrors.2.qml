import Qt 4.7

QtObject {
    id: root

    property variant a: 10
    property int x: 10
    property int test: a.x

    Component.onCompleted: {
        a = 11;
        a = root;
    }
}
