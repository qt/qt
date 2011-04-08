import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: container
    //ReflectiveProperties
    //TransferredProperties
    property variant target: {"y": -90, "x":12}
    property ParticleSystem system
    property bool show: true
    property int hardpointType: 0 //default is pea shooter - always bad.

    property Item targetObj: null
    property int damageDealt: 0
    onDamageDealtChanged: dealDamageTimer.start();
    Timer{
        id: dealDamageTimer
        interval: 16
        running: false
        repeat: false
        onTriggered: {targetObj.hp -= damageDealt; damageDealt = 0;}
    }
    width: 24
    height: 24
    function fireAt(targetArg){//Each implement own
        if(targetArg != null){
            hardpointLoader.item.fireAt(targetArg, container);
            targetObj = targetArg;
        }
    }
    Loader{
        id: hardpointLoader
        sourceComponent: {switch(hardpointType){
        case 1: laserComponent; break;
        case 2: blasterComponent; break;
        case 3: cannonComponent; break;
        default: emptyComponent;
        }}
    }
    Component{
        id: laserComponent
        LaserHardpoint{
            target: container.target
            system: container.system
            show: container.show
        }
    }
    Component{
        id: blasterComponent
        BlasterHardpoint{
            target: container.target
            system: container.system
            show: container.show
        }
    }
    Component{
        id: cannonComponent
        CannonHardpoint{
            target: container.target
            system: container.system
            show: container.show
        }
    }
    Component{
        id: emptyComponent
        Item {
            function fireAt(obj){
                console.log("Firing null weapon. It hurts.");
            }
        }
    }
}
