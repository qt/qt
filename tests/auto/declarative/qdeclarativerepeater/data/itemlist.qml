// This example demonstrates placing items in a view using
// a VisualItemModel

import Qt 4.7

Rectangle {
    color: "lightgray"
    width: 240
    height: 320

    function checkProperties() {
        testObject.error = false;
        if (testObject.useModel && view.model != itemModel) {
            console.log("model property incorrect");
            testObject.error = true;
        }
    }

    VisualItemModel {
        id: itemModel
        objectName: "itemModel"
        Rectangle {
            objectName: "item1"
            height: 50; width: 100; color: "#FFFEF0"
            Text { objectName: "text1"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            objectName: "item2"
            height: 50; width: 100; color: "#F0FFF7"
            Text { objectName: "text2"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
        Rectangle {
            objectName: "item3"
            height: 50; width: 100; color: "#F4F0FF"
            Text { objectName: "text3"; text: "index: " + parent.VisualItemModel.index; font.bold: true; anchors.centerIn: parent }
        }
    }

    Column {
        objectName: "container"
        Repeater {
            id: view
            objectName: "repeater"
            model: testObject.useModel ? itemModel : 0
        }
    }
}
