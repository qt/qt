import QtQuick 2.0
import Qt.labs.particles 2.0

Item {
    id: me
    //Reflective Properties
    width: shipLoader.width
    height: shipLoader.height
    //Transfered Properties
    property int hp: 100//shipLoader.item.maxHP
    property real dodge: shipLoader.item.initialDodge
    property ParticleSystem system
    property int targetIdx: 0
    property Item target: targets[targetIdx] == undefined?null:targets[targetIdx]
    Connections{
        target: me.target
        onHpChanged: if(me.target.hp<=0) me.targetIdx++;
    }
    property list<Item> targets
    property string shipParticle: "default"//Per team colors?
    property int gunType: 0
    property int shipType: 0
    Component{
        id: sloopComp
        Sloop{
            system: me.system
            target: me.target
            shipParticle: me.shipParticle
            gunType: me.gunType
            hp: me.hp
            dodge: me.dodge
        }
    }
    Component{
        id: frigateComp
        Frigate{
            system: me.system
            target: me.target
            shipParticle: me.shipParticle
            gunType: me.gunType
            hp: me.hp
            dodge: me.dodge
        }
    }
    Component{
        id: cruiserComp
        Cruiser{
            system: me.system
            target: me.target
            shipParticle: me.shipParticle
            gunType: me.gunType
            hp: me.hp
            dodge: me.dodge
        }
    }
    Component{
        id: dumbComp
        Item{
            property int maxHP: 0
            property int initialDodge: 0
        }
    }
    Loader{
        id: shipLoader
        sourceComponent:{ switch(shipType){
            case 1: sloopComp; break;
            case 2: frigateComp; break;
            case 3: cruiserComp; break;
            default: dumbComp;
        }
        }
    }
}
