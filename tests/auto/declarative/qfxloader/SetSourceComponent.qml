import Qt 4.6

Item {
    Component { id: Comp; Rectangle { width: 120; height: 60 } }
    Loader { sourceComponent: Comp }
}
