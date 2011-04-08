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
        particle: "laser"
        system: container.system
        anchors.fill: parent
        emitting: container.show
        shape: Ellipse{}
        speed: DirectedVector{ targetX: width/2; targetY: width/2; magnitude: -1; proportionalMagnitude: true }
        particleDuration: 1000
        particlesPerSecond: 64

        particleSize: 24
        particleSizeVariation: 8
        particleEndSize: 8
    }

    function fireAt(targetArg, hardpoint){
        if(targetArg.hp <= 0)
            return;
        //TODO: calculate hit and damage at target, which must be a Ship
        var offset = 0;
        if(Math.random() < 0.99){
            switch(targetArg.shipType){
            case 1: hardpoint.damageDealt += 16; break;
            case 2: hardpoint.damageDealt += 4; break;
            case 3: hardpoint.damageDealt += 8; break;
            default: hardpoint.damageDealt += 500; //Really effective against unregistered vessels
            }
        }else{//Misses with Lasers are really rare
            offset = Math.random() * 100;
        }
        target = container.mapFromItem(targetArg, offset + targetArg.width/2, offset + targetArg.height/2);
        emitter.pulse(0.10);
 //       console.log("Fire box: " +  Math.min(container.width/2, target.x) + "," + Math.min(container.height/2, target.y) + " " + (Math.max(container.width/2, target.x) - Math.min(container.width/2, target.x)) + "," + (Math.max(container.height/2, target.y) - Math.min(container.height/2, target.y)));
    }
    TrailEmitter{
        id: emitter
        particle: "laser"
        emitting: false
        system: container.system
        x: Math.min(container.width/2, target.x);
        width: Math.max(container.width/2, target.x) - x;
        y: Math.min(container.height/2, target.y);
        height: Math.max(container.height/2, target.y) - y;
        shape: Line{
            mirrored: (emitter.y < 0 || emitter.x < 0) && !(emitter.y < 0 && emitter.x < 0 )//I just want XOR
        }

        particleDuration: 1000
        particlesPerSecond: 8000
        maxParticles: 800
        particleSize: 16
        particleEndSize: 0

        speed: PointVector{xVariation: 4; yVariation: 4}
    }
}
