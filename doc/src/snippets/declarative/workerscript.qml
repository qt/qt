//![0]
import Qt 4.7

Rectangle {
    width: 300; height: 300

    Text {
        id: myText
        text: 'Click anywhere'
    }

    WorkerScript {
        id: myWorker
        source: "script.js"

        onMessage: myText.text = messageObject.reply
    }

    MouseArea {
        anchors.fill: parent
        onClicked: myWorker.sendMessage({ 'x': mouse.x, 'y': mouse.y })
    }
}
//![0]
