import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    property int maxHP: 100
    property int hp: maxHP
    property real initialDodge: 0.01
    property real dodge: initialDodge
    onHpChanged: if(hp <= 0) target = container;
    property ParticleSystem system//TODO: Ship abstraction
    property Item target: container
    property string shipParticle: "default"//Per team colors?
    property int gunType: 0
    width: 128
    height: 128
    TrailEmitter{
        //TODO: Cooler would be an 'orbiting' affector
        //TODO: On the subject, opacity and size should be grouped type 'overLife' if we can cram that in the particles
        system: container.system
        particle: container.shipParticle
        anchors.centerIn: parent
        width: 64
        height: 64
        shape: Ellipse{}

        particlesPerSecond: hp > 0 ?  hp * 1 + 20 : 0 
        particleDuration: 2400
        maxParticles: (maxHP * 1 + 20)*2.4

        particleSize: 48
        particleSizeVariation: 16
        particleEndSize: 16

        speed: AngleVector{angleVariation:360; magnitudeVariation: 32}
    }
    TrailEmitter{
        system: container.system
        particle: "cruiserArmor"
        anchors.fill: parent
        shape: Ellipse{ fill: false }
        emitting: hp>0
        
        particlesPerSecond: 16
        particleDuration: 2000

        particleSize: 48
        particleSizeVariation: 24

        SpriteGoal{
            id: destructor
            system: container.system
            active: container.hp <=0
            anchors.fill: parent
            particles: ["cruiserArmor"]
            goalState: "death"
//            jump: true
            onceOff: true
        }
    }

    Timer{
        id: fireControl
        property int next: Math.floor(Math.random() * 3) + 1
        interval: 800
        running: root.readySetGo
        repeat: true
        onTriggered:{
            if(next == 1){
                gun1.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }else if(next == 2){
                gun2.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }else if(next == 3){
                gun3.fireAt(container.target);
                next = Math.floor(Math.random() * 3) + 1;
            }
        }
    }

    Hardpoint{//TODO: Hardpoint abstraction
        x: 112 - 12 - 8*2
        y: 128 - 12 - 12*2
        id: gun1
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
    Hardpoint{
        x: 64 - 12
        y: 0 - 12 + 12*2
        id: gun2
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
    Hardpoint{
        x: 16 - 12 + 8*2
        y: 128 - 12 - 12*2
        id: gun3
        system: container.system
        show: hp > 0
        hardpointType: gunType
    }
}
