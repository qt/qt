import Qt 4.7

QtObject {
    property bool test: false
    Component.onCompleted: {
        try {
            console.log(sorryNoSuchProperty);
        } catch (e) {
            test = true;
        }
    }
}
