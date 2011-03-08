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
        id: uncloned
        width: 320
        height: 480
        startTime: 4000
        visible: !cloneMode
        PairedParticle{
            id: pairC
            pairs: [pairA, pairB]
            SpriteParticle{
                Sprite{
                    source: "Trails/content/particle2.png"
                }
            }
        }
        data:[TrailEmitter{
            system: uncloned
            y:480
            width: 320
            particlesPerSecond: 200
            particleDuration: 4000
            ySpeed: -120
        }]
    }
    ParticleSystem{
        id: cloneA
        width: 320
        height: 480
        visible: cloneMode
        z: 0
        PairedParticle{
            id: pairA
            SpriteParticle{
                Sprite{
                    source: "Trails/content/particle3.png"
                }
            }
        }
        ColoredParticle{
            id: dummy2
            image: "Trails/content/particle3.png"
        }
        emitters:TrailEmitter{
            particle: dummy2
            particlesPerSecond: 1
            particleDuration: 1000
            emitterX: -100
            emitterY: -100
        }
    }
    ParticleSystem{
        id: cloneB
        clip: true
        y: 140
        width: 320
        height: 200
        visible: cloneMode
        z: 1
        PairedParticle{
            id: pairB
            //pair: pairA 
            SpriteParticle{
                Sprite{
                    source: "Trails/content/particle.png"
                }
            }
        }
        ColoredParticle{
            id: dummy
            image: "Trails/content/particle3.png"
        }
        emitters:TrailEmitter{
            particle: dummy
            particlesPerSecond: 1
            particleDuration: 1000
            emitterX: -100
            emitterY: -100
        }
    }
}
