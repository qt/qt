import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    property int maxHP: 100
    property int hp: maxHP
    property real initialDodge: 0.5
    property real dodge: initialDodge
    property int blinkInterval: 800
    onHpChanged: if(hp <= 0) target = container;
    property ParticleSystem system//TODO: Ship abstraction
    property Item target: container
    property string shipParticle: "default"//Per team colors?
    property int gunType: 0
    width: 128
    height: 128
    TrailEmitter{
        id: emitter
        //TODO: Cooler would be an 'orbiting' affector
        //TODO: On the subject, opacity and size should be grouped type 'overLife' if we can cram that in the particles
        system: container.system
        particle: container.shipParticle
        shape: Ellipse{}

        particlesPerSecond: hp > 0 ?  hp + 20 : 0 
        particleDuration: blinkInterval
        maxParticles: (maxHP + 20)

        acceleration: AngleVector{angleVariation: 360; magnitude: 8}

        particleSize: 24
        particleEndSize: 4
        particleSizeVariation: 8
        width: 16
        height: 16
        x: 64
        y: 64
        Behavior on x{NumberAnimation{duration:blinkInterval}}
        Behavior on y{NumberAnimation{duration:blinkInterval}}
        Timer{
            interval: blinkInterval
            running: true
            repeat: true
            onTriggered: {
                emitter.x = Math.random() * 48 + 32
                emitter.y = Math.random() * 48 + 32
            }
        }
    }
    Hardpoint{
        anchors.centerIn: parent
        id: gun2
        system: container.system
        show: container.hp > 0
        hardpointType: gunType
    }
    Timer{
        id: fireControl
        interval: 800
        running: root.readySetGo
        repeat: true
        onTriggered:{
                gun2.fireAt(container.target);
        }
    }

}
