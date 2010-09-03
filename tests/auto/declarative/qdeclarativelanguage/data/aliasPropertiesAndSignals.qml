import Qt 4.7

QtObject {
    id: root

    property bool test: false
    property alias myalias: root.objectName
    signal go
    onGo: test = true

    Component.onCompleted: {
        root.go();
    }
}
