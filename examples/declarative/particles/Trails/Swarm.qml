import QtQuick 2.0
import Qt.labs.particles 2.0 as QLP

Rectangle{
    width: 200
    height: 200
    color: "black"
    QLP.ParticleSystem{ id: ps }
    QLP.ColoredParticle{
        system: ps
        particles: ["star1","star2"]
        anchors.fill: parent
        clip: true
        image: "content/star.png"
    }
    QLP.Swarm{ 
        system: ps
        leaders: ["star2"]; 
        anchors.fill: parent
        strength: 128
    }
    QLP.TrailEmitter{
        anchors.fill: parent
        system: ps
        particle: "star1"
        particlesPerSecond: 100
        particleDuration: 2000
    }
    QLP.TrailEmitter{
        anchors.fill: parent
        system: ps
        particle: "star2"
        particlesPerSecond: 0.4
        particleDuration: 10000
        particleSize: 64
        particleEndSize: 32
    }
}
