import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 200
    height: 200
    color: "black"
    ParticleSystem{
        id: ps
        anchors.fill: parent
        clip: true
        ColoredParticle{
            id: star1
            image: "content/star.png"
        }
        ColoredParticle{
            id: star2
            image: "content/particle.png"
        }
        affectors: Swarm{leaders: star2; strength: 128}
    }
    TrailEmitter{
        anchors.fill: parent
        system: ps
        particle: star1
        particlesPerSecond: 100
        particleDuration: 2000
    }
    TrailEmitter{
        anchors.fill: parent
        system: ps
        particle: star2
        particlesPerSecond: 1
        particleDuration: 8000
        particleSize: 32
        particleEndSize: 8
    }
}
