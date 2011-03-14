import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    ParticleSystem{ id: particles }
    SpriteParticle{
        system: particles
        Sprite{
            name: "snow"
            source: "flake-01.png"
            frames: 51
            duration: 40
        }
    }
    Drift{ 
        system: particles
        anchors.fill: parent
        xDrift: 400;
    }
    TrailEmitter{
        system: particles
        particlesPerSecond: 20
        particleDuration: 7000
        emitting: true
        ySpeed: 80
        ySpeedVariation: 40
        yAccel: 4
        particleSize: 20
        particleSizeVariation: 10
        width: parent.width
        height: 100
    }
}
