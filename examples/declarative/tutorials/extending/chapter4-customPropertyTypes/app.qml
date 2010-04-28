import Music 1.0
import Qt 4.7

Item {

    Musician {
        id: reddy
        name: "Reddy the Rocker"
        instrument: Instrument { type: "Guitar" }
    }

    Component.onCompleted: console.log("Reddy plays the " + reddy.instrument.type)
}
