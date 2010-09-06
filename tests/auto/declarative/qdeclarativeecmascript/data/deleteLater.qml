import Qt 4.7

QtObject {
    id: root
    property bool test: false

    Component.onCompleted: {
        try {
            root.deleteLater()
        } catch(e) {
            test = true;
        }
    }
}
