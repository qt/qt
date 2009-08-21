import Qt 4.6

Item {
    property bool explode : false

    Particles {
        id: particles
        width: 38
        height: 21
        lifeSpan: 1000
        lifeSpanDeviation: 0
        source: "pics/star.png"
        count: 0
        angle: 270
        angleDeviation: 360
        velocity: 100
        velocityDeviation: 20
        z: 100
        opacity: 0
        streamIn: false
    }
    states: [ State { name: "exploding"; when: explode == true
            PropertyChanges { target: particles; count: 200 }
            PropertyChanges { target: particles; opacity: 1 }
            PropertyChanges { target: particles; emitting: false } // i.e. emit only once
        }
    ]

}
