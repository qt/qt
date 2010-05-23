import Qt 4.7

QtObject {
    property list<QtObject> listProperty
    property int test: listProperty.length

    listProperty: [ QtObject{}, QtObject {} ]
}

