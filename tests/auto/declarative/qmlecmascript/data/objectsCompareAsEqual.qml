import Qt 4.6

Item {
    id: Root

    property var item: Child
    Item { id: Child }

    property bool test1: Child == Child
    property bool test2: Child.parent == Root
    property bool test3: Root != Child
    property bool test4: item == Child
    property bool test5: item != Root
}

