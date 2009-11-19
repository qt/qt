import Qt 4.6

QtObject {
    property int b: obj.prop.a

    property var prop;
    prop: QtObject {
        property int a: 10
    }
}

