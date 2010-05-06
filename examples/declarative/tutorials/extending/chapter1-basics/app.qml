import Music 1.0
import Qt 4.7

Rectangle {
    width: 300; height: 200

    Musician {
        id: aMusician
        name: "Reddy the Rocker"
        instrument: "Guitar"
    }

    Text {
        anchors.fill: parent
        text: aMusician.name + " plays the " + aMusician.instrument
    }
}
