import Qt 4.7

QtObject {
    property int b: obj.prop.a

    property variant prop;
    prop: QtObject {
        property int a: 10
    }
}

