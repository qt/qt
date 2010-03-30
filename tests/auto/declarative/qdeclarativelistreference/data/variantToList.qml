import Qt 4.6

QtObject {
    property list<QtObject> myList;
    myList: QtObject {}

    property var value: myList
    property int test: value.length
}

