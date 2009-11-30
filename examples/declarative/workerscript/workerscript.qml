import Qt 4.6

Rectangle {
    width: 480; height: 320;

    WorkerScript {
        id: myWorker
        source: "workerscript.js"

        onMessage: {
            print("Moved " + messageObject.xmove + " along the X axis.");
            print("Moved " + messageObject.ymove + " along the Y axis.");
            print("Moved " + messageObject.move + " pixels.");
        }
    }

    Rectangle {
        width: 200; height: 200
        anchors.left: parent.left
        anchors.leftMargin: 20
        color: "red"

        MouseRegion { 
            anchors.fill: parent
            onClicked: myWorker.sendMessage( { rectangle: "red", x: mouse.x, y: mouse.y } );
        }
    }

    Rectangle {
        width: 200; height: 200
        anchors.right: parent.right
        anchors.rightMargin: 20
        color: "blue"

        MouseRegion { 
            anchors.fill: parent
            onClicked: myWorker.sendMessage( { rectangle: "blue", x: mouse.x, y: mouse.y } );
        }
    }

    Text {
        text: "Click a Rectangle!"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
    }
}
