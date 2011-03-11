import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    id: page
    TrailEmitter{
        particle: sp
        system: particles
        particlesPerSecond: 20
        particleDuration: 7000
        emitting: true
        ySpeed: 80
        ySpeedVariation: 40
        yAccel: 4
        particleSize: 20
        particleSizeVariation: 10
        anchors.fill: parent
        anchors.bottomMargin: 200
    }
    ParticleSystem{
        id: particles
        anchors.fill: parent
        particles: SpriteParticle{
            id: sp
            sprites: Sprite{
                name: "snow"
                source: "flake-01.png"
                frames: 51
                duration: 40
            }
        }
        affectors: [Wander{ xVariance: 30; pace: 100; },
            Zone{
                x: 0
                y: page.height - 200
                width: page.width
                height: 200
                Kill{}
            }]
    }
}
