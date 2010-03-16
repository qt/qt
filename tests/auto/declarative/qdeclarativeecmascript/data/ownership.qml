import Qt 4.6

QtObject {
    Component.onCompleted: { var a = getObject(); a = null; }
}
