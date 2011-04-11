import QtQuick 2.0
import Qt.labs.particles 2.0

Rectangle{
    id: root
    color: "black"
    width: 640
    height: 480
    ParticleSystem{
        id: sys
    }
    ColoredParticle{
        system: sys
        image: "content/particle.png"
        color: "white"
        colorVariation: 1.0
        alpha: 0.1
    }
    Component{
        id: emitterComp
        TrailEmitter{
            id: container
            TrailEmitter{
                id: emitMore
                system: sys
                emitting: true
                particlesPerSecond: 128
                particleDuration: 600
                particleSize: 16
                particleEndSize: 8
                speed: AngleVector{angleVariation:360; magnitude: 60}
            }

            property int life: 2600
            property real targetX: 0
            property real targetY: 0
            function go(){
                xAnim.start();
                yAnim.start();
                container.emitting = true
            }
            system: sys
            emitting: true
            particlesPerSecond: 64
            particleDuration: 600
            particleSize: 24
            particleEndSize: 8
            NumberAnimation on x{
                id: xAnim;
                to: targetX
                duration: life
                running: false
            }
            NumberAnimation on y{
                id: yAnim;
                to: targetY
                duration: life
                running: false
            }
            Timer{
                interval: life
                running: true
                onTriggered: container.destroy();
            }
        }
    }
    MouseArea{
        anchors.fill: parent
        onClicked:{
            for(var i=0; i<16; i++){
                var obj = emitterComp.createObject(root);
                obj.x = mouse.x
                obj.y = mouse.y
                obj.targetX = Math.random() * 640
                obj.targetY = Math.random() * 480
                obj.life = Math.round(Math.random() * 2400) + 200
                obj.go();
            }
        }
    }
}
