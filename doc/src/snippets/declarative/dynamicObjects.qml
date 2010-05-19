import Qt 4.7

//![0]
Rectangle {
    id: rootItem
    width: 300
    height: 300

    Component {
        id: rectComponent

        Rectangle {
            id: rect
            width: 40; height: 40;
            color: "red"

            NumberAnimation on opacity { from: 1; to: 0; duration: 1000 }

            Component.onCompleted: rect.destroy(1000);
        }
    }

    function createRectangle() {
        var object = rectComponent.createObject(rootItem);
    } 

    Component.onCompleted: createRectangle()
}
//![0]
