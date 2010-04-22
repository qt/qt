import Qt 4.7

QtObject {
    property list<QtObject> myList;
    myList: QtObject {}

    property variant value: myList
    property int test: value.length
}

