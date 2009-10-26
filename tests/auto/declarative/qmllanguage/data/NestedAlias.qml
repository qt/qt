import Qt 4.6

Object {
    property Object o1
    property Object o2

    property alias a: object2.a

    o1: Object { id: object1 }
    o2: Object { 
        id: object2 
        property int a: 1923
    }
}
