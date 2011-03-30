import QtQuick 2.0
import Qt.labs.particles 2.0
import "script.js" as Script

Item{
    id: root
    width: 640
    height: 480
    Timer{
        id: startTimer
        running: true
        repeat: false
        interval: 6000
        onTriggered: {sys.running = true; loading.opacity = 0.0}
    }
    Item{
        id: loading
        anchors.fill: parent
        Text{
            anchors.centerIn: parent
            text: "Loading"
        }
    }
    ParticleSystem{ 
        id: sys;
        running: false
    }
    TrailEmitter{
        system: sys
        particle: "A"
        height: parent.height
        speed: PointVector{ x: 64; xVariation: 16 }
        particlesPerSecond: 0.5
        particleDuration: 8000 //TODO: A -1 or something which does 'infinite'? (but need disable fade first)
    }
    ModelParticle{
        id: mp
        z: 0
        system: sys
        particles: ["A"]
        model: RssModel{tags:"particle,particles"}
        delegate: Rectangle{
            id: container
            border.width: 2
            property real myRand: Math.random();
            z: Math.floor(myRand * 100)
            rotation: -10 + (myRand * 20)
            width: 132
            height: 132
            function manage()
            {
                if(state == "selected"){
                    console.log("Taking " + index);
                    mp.take(index);
                }else{
                    console.log("Returning " +index);
                    mp.recover(index);
                }
            }
            Image{
                id: img
                anchors.centerIn: parent
                smooth: true; source: "http://" + Script.getImagePath(content); cache: true
                fillMode: Image.PreserveAspectFit; 
                width: parent.width-4; height: parent.height-4
            }
            Text{
                anchors.bottom: parent.bottom
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                text: title
                color: "black"
            }
            MouseArea{
                anchors.fill: parent
                onClicked: container.state == "selected" ? container.state = "" : container.state = "selected"
            }
            states: State{
                name: "selected"
                ParentChange{
                    target: container
                    parent: root
                    x: 0
                    y: 0
                }
                PropertyChanges{
                    target: container
                    width: root.width
                    height: root.height
                    z: 101
                    opacity: 1
                    rotation: 0
                }
            }
            transitions: Transition{
                to: "selected"
                reversible: true
                SequentialAnimation{
                    ScriptAction{script: container.manage();}
                    ParallelAnimation{
                        ParentAnimation{NumberAnimation{ properties: "x,y" }}//Doesn't work, particles takes control of x,y instantly
                        NumberAnimation{ properties: "width, height, z, rotation" }
                    }
                }
            }
        }
    }
}
