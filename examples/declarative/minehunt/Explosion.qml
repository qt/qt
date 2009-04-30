Item {
    properties: Property {
        name: "explode"
        type: "Bool"
        value: false
    }
    Particles {
        width: 38
        height: 21
        lifeSpan: 3600000
        lifeSpanDeviation: 0
        source: "pics/star.png"
        count: 200
        angle: 270
        angleDeviation: 360
        velocity: 100
        velocityDeviation: 20
        z: 100
        emitting: explode
    }
}
