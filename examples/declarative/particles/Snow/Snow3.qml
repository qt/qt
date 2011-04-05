import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    width: 360
    height: 540
    id: root
    ParticleSystem{ id: particles }
    SpriteParticle{
        system: particles
        sprites: Sprite{
            name: "snow"
            source: "flake-01.png"
            frames: 51
            duration: 40
        }
    }
    Drift{
        system: particles
        anchors.fill: parent
        xDrift: 200
    } 
    SpeedLimit{
        system: particles
        anchors.fill: parent
        speedLimit: 100
    }
    TrailEmitter{
        system: particles
        particlesPerSecond: 20
        particleDuration: 7000
        emitting: true
        speed: PointVector{ y:80; yVariation: 40; }
        acceleration: PointVector{ y: 4 }
        particleSize: 20
        particleSizeVariation: 10
        width: parent.width
        height: 40
    }
}
