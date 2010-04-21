import Qt 4.7

QtObject {
    property list<MyType> myList

    myList: [ MyType { a: 1 },
              MyType { a: 9 } ]

}
