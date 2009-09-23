import Test 1.0

MyQmlObject {
    id: Root
    property alias aliasObject: Root.qmlobjectProperty

    qmlobjectProperty: MyQmlObject { value : 10 }
}
