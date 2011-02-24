import Qt.labs.particles 2.0
import QtQuick 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{
        id: particles
        particles: ColoredParticle{
            colorVariation: 0.5
            additive: 1

            image: "particle.png"
            colortable: "colortable.png"
        }
        emitters: TrailEmitter{
            particlesPerSecond: 500
            particleDuration: 2000

            emitterY: root.height / 2 + Math.sin(t * 2) * root.height * 0.3
            emitterX: root.width / 2 + Math.cos(t) * root.width * 0.3
            property real t;

            NumberAnimation on t {
                from: 0; to: Math.PI * 2; duration: 10000; loops: Animation.Infinite
            }

            speedFromMovement: 20

            xAccelVariation: 5
            yAccelVariation: 5

            xSpeedVariation: 5
            ySpeedVariation: 5

            particleSize: 2
            particleEndSize: 8
            particleSizeVariation: 8
        }
    }


}
