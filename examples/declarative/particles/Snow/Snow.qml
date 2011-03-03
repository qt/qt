import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    id: page
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
        emitters: TrailEmitter{
            particle: sp
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
            emitterX: page.width/2
            emitterY: 50
            emitterXVariation: page.width/2
            emitterYVariation: 50
        }
        affectors: [Wander{ xVariance: 30; pace: 100; },
            Zone{
                x: 0
                y: 3 * page.height/4
                width: page.width
                height: page.height/4
                Kill{}
            }]
    }
}
