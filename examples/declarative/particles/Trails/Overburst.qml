import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    color: "black"
    width: 360
    height: 540
    ParticleSystem{ id: sys }
    ColoredParticle{
        system: sys
        id: cp
        image: "content/particle.png"
        colorVariation: 0.4
        additive: 1
    }
    TrailEmitter{
        //burst on click
        id: bursty
        system: sys
        emitting: ma.pressed
        x: ma.mouseX
        y: ma.mouseY
        particlesPerSecond: 16000
        particleDuration: 1000
        maxParticleCount: 4000
        acceleration: AngleVector{angleVariation: 360; magnitude: 360; }
        particleSize: 8
        particleEndSize: 16
        particleSizeVariation: 4
    }
    MouseArea{
        anchors.fill: parent
        id: ma
    }
}
