import Qt 4.6

Item {
    Component { id: comp; Rectangle { width: 100; height: 50 } }
    Loader { sourceComponent: comp }
}
