import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    color: "black"
    ParticleSystem{
        id: particles
        anchors.fill: parent
        particles: ColoredParticle{
            id: sp
            image: "content/star.png"
            colorVariation: 0.2
        }
        emitters: TrailEmitter{
            particle: sp
            particlesPerSecond: 2000
            particleDuration: 2000
            emitting: true
            particleSize: 40
            particleSizeVariation: 10
            emitterX: width/2
            emitterY: height/2
            emitterXVariation: width/2
            emitterYVariation: height/2
        }
        affectors: Wander{ xVariance: 30; yVariance: 30; pace: 10; }
    }
}
