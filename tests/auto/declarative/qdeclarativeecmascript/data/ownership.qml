import Qt 4.7

QtObject {
    Component.onCompleted: { var a = getObject(); a = null; }
}
