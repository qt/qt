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
        emitting: false
        particlesPerSecond: 2000
        particleDuration: 500
        acceleration: AngleVector{ angle: 90; angleVariation: 360; magnitude: 640; }
        particleSize: 8
        particleEndSize: 16
        particleSizeVariation: 4
    }
    TrailEmitter{
        system: sys
        speedFromMovement: 4.0
        emitting: ma.pressed
        x: ma.mouseX
        y: ma.mouseY
        particlesPerSecond: 400
        particleDuration: 2000
        acceleration: AngleVector{ angle: 90; angleVariation: 22; magnitude: 32; }
        particleSize: 8
        particleEndSize: 16
        particleSizeVariation: 8
    }
    MouseArea{
        id: ma
        anchors.fill: parent
        onPressed: {bursty.x = mouse.x; bursty.y = mouse.y; bursty.burst(0.1);}
        onReleased: {bursty.x = mouse.x; bursty.y = mouse.y; bursty.burst(0.1);}
    }
}
