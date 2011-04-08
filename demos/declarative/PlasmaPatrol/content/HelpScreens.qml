import QtQuick 2.0
import Qt.labs.particles 2.0

SequentialLoader {
    id: hLdr
    signal exitDesired
    Component.onCompleted: advance();
    ParticleSystem{ id: helpSystem }
    PlasmaPatrolParticles{ sys: helpSystem }
    pages: [
        Component{Item{
            id: story
            Text{
                color: "white"
                text: "Story"
                font.pixelSize: 48
            }
            /*
            Flickable{
                y: 60
                width: 360
                height: 500
                contentHeight: txt1.height
                contentWidth: 360//TODO: Less magic numbers?
                */
                Text{
                    id: txt1
                    color: "white"
                    y: 60
                    font.pixelSize: 18
                    text: "
In a remote nebula, a race of energy beings formed and lived prosperous lives for millenia. Until the schism - when they became constantly at each other's energy-throats. War soon followed, crippling both sides, until a truce was formed. But while governments knew the desparate need for peace, the soldiers in the ion-field were still filled with rampant bloodlust. On the border, patrols are constantly engaging in minor skirmishes whenever they cross paths. 

You must select one such patrol unit for the border, heading into an inevitable skirmish, in Plasma Patrol: the game of energy being spaceship combat!
                    "
                    width: 360
                    wrapMode: Text.WordWrap
                }
           // }
            Button{
                x: 20
                y: 560
                height: 40
                width: 120
                text: "Next"
                onClicked: hLdr.advance();
            }
            Button{
                x: 220
                y: 560
                height: 40
                width: 120
                text: "Menu"
                onClicked: hLdr.exitDesired();
            }
        }},
        Component{Item{
            id: ships
            Text{
                color: "white"
                text: "Vessels"
                font.pixelSize: 48
            }
            Column{
                spacing: 16
                y: 60
                Row{
                    height: 128
                    Sloop{
                        system: helpSystem
                    }
                    Text{
                        text: "The nimble sloop"
                        color: "white"
                        font.pixelSize: 18
                    }
                }
                Row{
                    height: 128
                    Frigate{
                        system: helpSystem
                    }
                    Text{
                        text: "The versitile shield frigate"
                        color: "white"
                        font.pixelSize: 18
                    }
                }
                Row{
                    height: 128
                    Cruiser{
                        system: helpSystem
                    }
                    Text{
                        text: "The armored cruiser"
                        color: "white"
                        font.pixelSize: 18
                    }
                }
            }
            Button{
                x: 20
                y: 560
                height: 40
                width: 120
                text: "Next"
                onClicked: hLdr.advance();
            }
            Button{
                x: 220
                y: 560
                height: 40
                width: 120
                text: "Menu"
                onClicked: hLdr.exitDesired();
            }
        }},
        Component{Item{
            id: guns
            Text{
                color: "white"
                text: "Hardpoints"
                font.pixelSize: 48
            }
            Column{
                spacing: 16
                y: 60
                Row{
                    height: 128
                    LaserHardpoint{
                        system: helpSystem
                    }
                    Text{
                        text: "The laser hardpoint almost always hits the target, even the nimble sloop, but loses much of its potency against the frigate's shields"
                        width: 332
                        wrapMode: Text.WordWrap
                        color: "white"
                        font.pixelSize: 18
                    }
                }
                Row{
                    height: 128
                    BlasterHardpoint{
                        system: helpSystem
                    }
                    Text{
                        text: "The blaster passes right through the frigate's shields but loses much of its impact against the armor of the cruiser"
                        width: 332
                        wrapMode: Text.WordWrap
                        color: "white"
                        font.pixelSize: 18
                    }
                }
                Row{
                    height: 128
                    CannonHardpoint{
                        system: helpSystem
                    }
                    Text{
                        text: "The cannon has poor accuracy, often missing the nimble sloop, but can punch right through the armor of the cruiser"
                        width: 332
                        wrapMode: Text.WordWrap
                        color: "white"
                        font.pixelSize: 18
                    }
                }
            }
            Button{
                x: 20
                y: 560
                height: 40
                width: 120
                text: "Next"
                onClicked: hLdr.advance();
            }
            Button{
                x: 220
                y: 560
                height: 40
                width: 120
                text: "Menu"
                onClicked: hLdr.exitDesired();
            }
        }},
        Component{Item{
            id: strategy
            Text{
                color: "white"
                text: "Strategy"
                font.pixelSize: 48
            }
            Flickable{
                y: 60
                width: 360
                height: 500
                contentHeight: txt1.height
                contentWidth: 360//TODO: Less magic numbers?
                Text{
                    id: txt1
                    color: "white"
                    font.pixelSize: 18
                    text: "
Basic Strategy: Good luck, have fun - don't die.
More to come after thorough playtesting.
                    "
                    width: 360
                    wrapMode: Text.WordWrap
                }
            }
            Button{
                x: 20
                y: 560
                height: 40
                width: 120
                text: "Story"
                onClicked: {hLdr.at=0; hLdr.advance();}
            }
            Button{
                x: 220
                y: 560
                height: 40
                width: 120
                text: "Menu"
                onClicked: hLdr.exitDesired();
            }
        }}
    ]
}
