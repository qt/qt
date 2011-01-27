import QtQuick 1.0

Item{
    id: root
    property QtObject declarativerectangle : null
    property QtObject declarativeitem : null
    Component{id: a; Rectangle{} }
    Component{
        id: b
        Item{
            property bool testBool: false
            property int testInt: null
            property QtObject testObject: null
        }
    }

    Component.onCompleted: {
        root.declarativerectangle = a.createObject(root, {"x":17,"y":17, "color":"white"});
        root.declarativeitem = b.createObject(root, {"x":17,"y":17,"testBool":true,"testInt":17,"testObject":root});
    }
}
