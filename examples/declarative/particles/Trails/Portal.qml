import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    id: root
    width: 360
    height: 540
    color: "black"
    Image{
        anchors.fill: parent
        source: "content/portal_bg.png"
    }
    ParticleSystem{ 
        id: particles 
        startTime: 2000
    }
    ColoredParticle{
        particles: ["center","edge"]
        anchors.fill: parent
        system: particles
        image: "content/particle.png"
        colorVariation: 0.1
        color: "lightsteelblue"
        additive: 1
    }
    TrailEmitter{
        anchors.fill: parent
        particle: "center"
        system: particles
        particlesPerSecond: 200
        particleDuration: 2000
        emitting: true
        particleSize: 20
        particleSizeVariation: 2
        particleEndSize: 0
        shape: Ellipse{fill: false}
        speed: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            proportionalMagnitude: true
            magnitude: 0.5
        }
    }
    TrailEmitter{
        anchors.fill: parent
        particle: "edge"
        system: particles
        particlesPerSecond: 2000
        particleDuration: 2000
        emitting: true
        particleSize: 20
        particleSizeVariation: 2
        particleEndSize: 0
        shape: Ellipse{fill: false}
        speed: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            proportionalMagnitude: true
            magnitude: 0.1
            magnitudeVariation: 0.1
        }
        acceleration: DirectedVector{
            targetX: root.width/2 
            targetY: root.height/2
            targetVariation: 200
            proportionalMagnitude: true
            magnitude: 0.1
            magnitudeVariation: 0.1
        }
    }
}
