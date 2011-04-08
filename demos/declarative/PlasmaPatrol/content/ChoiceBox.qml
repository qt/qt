import QtQuick 2.0
import Qt.labs.particles 2.0

Item{
    id: container
    width: 360
    height: 160
    property ParticleSystem system
    Ship{
        id: nully
        system: system
    }
    property Item target: nully
    /*
    Component.onCompleted:{
        container.target.shipType = 1
        container.target.gunType = 1
    }
    */
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.height
        spacing: 8
        Button{
            width: 80
            height: 80
            anchors.verticalCenter: parent.verticalCenter
            text: "Cycle\nShip"
            onClicked: {
                var nextVal = container.target.shipType;
                if(nextVal == 3)
                    nextVal = 1;
                else 
                    nextVal++;
                container.target.shipType = nextVal;
            }
        }
        Item{
            width: 128
            height: 128
            anchors.verticalCenter: parent.verticalCenter
            Ship{
                hp: 20
                anchors.centerIn: parent
                shipType: container.target.shipType
                gunType: container.target.gunType
                system: container.system
            }
        }
        Button{
            width: 80
            height: 80
            anchors.verticalCenter: parent.verticalCenter
            text: "Cycle\nGun"
            onClicked: {
                var nextVal = container.target.gunType;
                if(nextVal == 3)
                    nextVal = 1;
                else 
                    nextVal++;
                container.target.gunType = nextVal;
            }
        }
    }
}
