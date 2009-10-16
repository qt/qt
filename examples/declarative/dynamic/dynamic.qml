import Qt 4.6

Rectangle {
    id: page
    width: 800
    height: 800 
    color: "black"
    Script { source: "dynamic.js" }

    property bool extendStars: false
    property var fourthBox: undefined

    Item { id: targetItem; x: 100; y: 100; }
    Item { id: targetItem2; x: 0; y: 300; }

    Rectangle {
        id: rect
        width: 100
        height: 100
        color: "green"

        MouseRegion {
            anchors.fill: parent
            onClicked: { var a = createWithComponent(); }
        }
    }

    Rectangle {
        id: rect2
        width: 100
        height: 100
        y: 100
        color: "red"

        MouseRegion {
            anchors.fill:parent
            onClicked: { destroyDynamicObject(); }
        }
    }

    Rectangle {
        id: rect3
        width: 100
        height: 100
        y: 200
        color: "blue"

        MouseRegion {
            anchors.fill: parent
            onClicked: { 
                if (fourthBox == null || fourthBox == undefined) {
                    var a = createQml(targetItem2);
                    if (a != null) {
                        a.parent = targetItem2;//BUG: this should happen automatically
                        fourthBox = a;
                        print(a.toStr());
                        extendStars = true;
                    }
                } else {
                    fourthBox.state = 'dying';
                    fourthBox.destroy(500);
                    fourthBox = null;
                    extendStars = false;
                }
            }
        }
    }

    Particles {
        x: 0 
        y: 0
        z: 10
        count: 20
        lifeSpan: 500
        width: 100
        height: if (extendStars) { 400; } else { 300; }
        source: "star.png"
    }
}
