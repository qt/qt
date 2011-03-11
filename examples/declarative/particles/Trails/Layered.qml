import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    id: root
    width: 320
    height: 480
    color: "darkblue"
    property bool cloneMode: false
    MouseArea{
        anchors.fill: parent
        onClicked: cloneMode = !cloneMode;
    }
    ParticleSystem{
        id: sys
        startTime: 4000
    }
    TrailEmitter{
        system: sys
        y:480
        width: 320
        particlesPerSecond: 200
        particleDuration: 4000
        ySpeed: -120
    }
    SpriteParticle{
        system: sys
        visible: !cloneMode
        Sprite{
            source: "content/particle2.png"
        }
    }
    SpriteParticle{
        system: sys
        visible: cloneMode
        z: 0
        Sprite{
            source: "content/particle3.png"
        }
    }
    SpriteParticle{
        system: sys
        clip: true
        visible: cloneMode
        y: 120
        height: 240
        width: root.width
        z: 1
        Sprite{
            source: "content/particle.png"
        }
    }
}
