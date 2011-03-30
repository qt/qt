import QtQuick 2.0
import Qt.labs.particles 2.0

Item{
    id: root
    width: 240
    height: 240
    property bool inGrid: false
    ParticleSystem{ id: sys }
    TrailEmitter{
        system: sys
        id: burster;
        emitting: false
        particlesPerSecond: 1000
        particleDuration: 500
        speed: PointVector{xVariation: 400; yVariation: 400}
        anchors.centerIn: parent
        Timer{
            interval: 1000
            running: true
            repeat: false
            onTriggered: burster.burst(0.1);
        }
        Timer{
            interval: 2000
            running: true
            repeat: false
            onTriggered: {inGrid = true;}// sys.running = false;}
        }
    }
    ColoredParticle{
        system: sys
        image: "../Trails/content/particle.png"
        color: "black"
        colorVariation: 0.0
        additive: 0
    }
    GridView{ id: grid; cellWidth: 40; cellHeight: 40
        model: theModel.parts.grid
        width: 120
        height: 120
    }
    ModelParticle{
        system: sys
        model: theModel.parts.particles
    }
    Friction{
        system: sys
        factor: 1
    }
    Stasis{
        system: sys
        targetLife: 400
    }
    VisualDataModel{
        id: theModel
        delegate: Delegate2{}
        model: ListModel{
            ListElement{
                w: 40
                h: 20
                col: "forestgreen"
            }
            ListElement{
                w: 20
                h: 40
                col: "salmon"
            }
            ListElement{
                w: 20
                h: 20
                col: "lightsteelblue"
            }
            ListElement{
                w: 40
                h: 40
                col: "goldenrod"
            }
            ListElement{
                w: 40
                h: 20
                col: "forestgreen"
            }
            ListElement{
                w: 20
                h: 40
                col: "salmon"
            }
            ListElement{
                w: 20
                h: 20
                col: "lightsteelblue"
            }
            ListElement{
                w: 40
                h: 40
                col: "goldenrod"
            }
            ListElement{
                w: 0
                h: 0
                col: "white"//Hack because add isn't working well with old stuff
            }
        }
    }
}
