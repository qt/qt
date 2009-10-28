import Qt 4.6

Object {
    property int b: obj.prop.a

    property var prop;
    prop: Object {
        property int a: 10
    }
}

