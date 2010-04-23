import Music 1.0
import Qt 4.7

Rectangle {
    width: 200; height: 200

    Musician {
        id: reddy
        name: "Reddy the Rocker"
        instrument: "Guitar"
    }

    Musician {
        id: craig
        name: "Craig the Copycat"
        instrument: reddy.instrument
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            reddy.perform()
            craig.perform()

            reddy.instrument = "Drums"

            reddy.perform()
            craig.perform()
        }
    }
}
