import Qt 4.6

Object {
    property list<Object> listProperty
    property int test: listProperty.length

    listProperty: [ Object{}, Object {} ]
}

