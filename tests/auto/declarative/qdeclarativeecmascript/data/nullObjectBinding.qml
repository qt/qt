import Qt 4.7

QtObject {
    property QtObject test
    test: if (1) model
    property ListModel model
}

