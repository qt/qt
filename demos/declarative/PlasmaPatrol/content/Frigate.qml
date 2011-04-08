import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    property int maxHP: 100
    property int hp: maxHP
    property real initialDodge: 0.2
    property real dodge: initialDodge
    onHpChanged: if(hp <= 0) target = container;
    property ParticleSystem system//TODO: Ship abstraction
    property Item target: container
    property string shipParticle: "default"//Per team colors?
    property int gunType: 0
    width: 128
    height: 128
    TrailEmitter{
        system: container.system
        particle: "frigateShield"
        anchors.centerIn: parent
        particleSize: 92
        particlesPerSecond: 1
        particleDuration: 4800
        emitting: hp > 0
    }
    TrailEmitter{
        system: container.system
        particle: container.shipParticle
        anchors.centerIn: parent
        width: 64 
        height: 16
        shape: Ellipse{}

        particleSize: 16
        particleSizeVariation: 8
        particleEndSize: 8
        particlesPerSecond: hp > 0 ?  hp * 1 + 20 : 0 
        particleDuration: 1200
        maxParticles: (maxHP * 1 + 20)*2
    }
    Timer{
        id: fireControl
        property int next: Math.floor(Math.random() * 2) + 1
        interval: 800
        running: root.readySetGo
        repeat: true
        onTriggered:{
            if(next == 1){
                gun1.fireAt(container.target);
                next = Math.floor(Math.random() * 2) + 1;
            }else if(next == 2){
                gun2.fireAt(container.target);
                next = Math.floor(Math.random() * 2) + 1;
            }
        }
    }

    Hardpoint{
        x: 128 - 32 - 12
        y: 64 - 12
        id: gun1
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
    Hardpoint{
        x: 0 + 32 - 12
        y: 64 - 12
        id: gun2
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
}
