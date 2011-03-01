import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    id: root
    ParticleSystem{
        id: particles
        anchors.fill: parent
        particles:SpriteParticle{
            sprite: Sprite{
                name: "snow"
                source: "flake-01.png"
                frames: 51
                duration: 40
            }
        }
        affectors:[Drift{
            xDrift: 200
        }, SpeedLimit{
            speedLimit: 100
        }]
        emitters:TrailEmitter{
            particlesPerSecond: 20
            particleDuration: 7000
            emitting: true
            xSpeed: 0
            ySpeed: 80
            xSpeedVariation: 20
            ySpeedVariation: 40
            xAccel: 0
            yAccel: 4
            particleSize: 20
            particleSizeVariation: 10
            emitterX: width/2
            emitterY: 50
            emitterXVariation: width/2
            emitterYVariation: 50
        }
    }
}
