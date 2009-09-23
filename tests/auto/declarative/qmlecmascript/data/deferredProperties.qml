import Qt.test 1.0

MyDeferredObject {
    value: 10
    objectProperty: MyQmlObject {}
    objectProperty2: MyQmlObject { id: blah }
}
