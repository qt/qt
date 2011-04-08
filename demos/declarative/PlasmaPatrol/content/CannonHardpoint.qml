import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    property variant target: {"y": -90, "x":12}
    property ParticleSystem system
    property bool show: true

    width: 24
    height: 24
    TrailEmitter{
        id: visualization
        particle: "cannon"
        emitting: container.show
        system: container.system
        anchors.centerIn: parent
        particleDuration: 2000
        particlesPerSecond: 1

        particleSize: 4
        particleEndSize: 0
    }

    function fireAt(targetArg, hardpoint){
        target = container.mapFromItem(targetArg, targetArg.width/2, targetArg.height/2);
        if(container.hp <= 0 || targetArg.hp <= 0)
            return;
        //TODO: calculate hit and damage at target, which must be a Ship
        var hit = Math.random() > targetArg.dodge
        if(hit){
            switch(targetArg.shipType){
            case 1: hardpoint.damageDealt += 8; break;
            case 2: hardpoint.damageDealt += 10; break;
            case 3: hardpoint.damageDealt += 16; break;
            default: hardpoint.damageDealt += 1000;
            }
        }
        emitter.burst(1);
    }
    TrailEmitter{
        id: emitter
        particle: "cannon"
        emitting: false
        system: container.system
        anchors.centerIn: parent

        particleDuration: 1000
        particlesPerSecond: 1
        particleSize: 8
        particleEndSize: 4
        speed: DirectedVector{
            id: blastVector
            targetX: target.x; targetY: target.y; magnitude: 1.1; proportionalMagnitude: true
        }
    }
}
