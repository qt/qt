import QtQuick 2.0
import Qt.labs.particles 2.0
import "script.js" as Script
import "../../modelviews/listview/content" as OtherDemo

Item{
    id: root
    width: 400
    height: 400
    Rectangle{
        anchors.fill: parent
        color: "lightsteelblue"
    }
    ParticleSystem{ 
        id: sys;
    }
    TrailEmitter{
        system: sys
        particle: "A"
        width: parent.width/2
        x: parent.width/4
        y:parent.height
        speed: PointVector{ y: -64; yVariation: 16 }
        particlesPerSecond: 1
        particleDuration: 8000
    }
    Drift{
        system: sys
        xDrift: 200
    }
    ModelParticle{
        id: mp
        z: 0
        system: sys
        particles: ["A"]
        model: OtherDemo.RecipesModel{}
        delegate: ExpandingDelegate{}
    }
}
