import Qt 4.6

QtObject {
    property list<QtObject> listProperty
    property int test: listProperty.length

    listProperty: [ QtObject{}, QtObject {} ]
}

