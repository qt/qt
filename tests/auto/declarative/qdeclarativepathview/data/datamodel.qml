import Qt 4.7

PathView {
    id: pathview
    objectName: "pathview"
    width: 240; height: 320
    pathItemCount: testObject.pathItemCount

    function checkProperties() {
        testObject.error = false;
        if (testObject.useModel && pathview.model != testData) {
            console.log("model property incorrect");
            testObject.error = true;
        }
    }

    model: testObject.useModel ? testData : 0

    delegate: Component {
        id: myDelegate
        Rectangle {
            id: wrapper
            objectName: "wrapper"
            width: 20; height: 20; color: name
            Text {
                objectName: "myText"
                text: name
            }
        }
    }

    path: Path {
        startX: 120; startY: 20;
        PathLine { x: 120; y: 300 }
    }
}
