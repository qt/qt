import Music 1.0
import Qt 4.7

Rectangle {
    width: 200; height: 200

    Musician {
        id: aMusician
        name: "Reddy the Rocker"
        instrument: "Guitar"

        onPerformanceEnded: console.log("The performance has now ended")
    }

    MouseArea {
        anchors.fill: parent
        onClicked: aMusician.perform()
    }
}
