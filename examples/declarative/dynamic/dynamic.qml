import Qt 4.6

Item {
    id: window
    //This is a desktop-sized example
    width: 1024; height: 512
    property int activeSuns: 0

    // sky
    Rectangle { id: sky
        anchors { left: parent.left; top: parent.top; right: toolbox.right; bottom: parent.verticalCenter }
        gradient: Gradient {
            GradientStop { id: stopA; position: 0.0; color: "#0E1533" }
            GradientStop { id: stopB; position: 1.0; color: "#437284" }
        }
    }

    // stars (when there's no sun)
    Particles { id: stars
        x: 0; y: 0; width: parent.width; height: parent.height/2
        source: "images/star.png"; angleDeviation: 360; velocity: 0
        velocityDeviation: 0; count: parent.width / 10; fadeInDuration: 2800
        opacity: 1
    }

    // ground, which has a z such that the sun can set behind it
    Rectangle { id: ground
        z: 2
        anchors { left: parent.left; top: parent.verticalCenter; right: toolbox.right; bottom: parent.bottom }
        gradient: Gradient {
            GradientStop { position: 0.0; color: "ForestGreen" }
            GradientStop { position: 1.0; color: "DarkGreen" }
        }
    }

    //Day state, for when you place a sun
    states: State { name: "Day"; when: window.activeSuns > 0
        PropertyChanges { target: stopA; color: "DeepSkyBlue"}
        PropertyChanges { target: stopB; color: "SkyBlue"}
        PropertyChanges { target: stars; opacity: 0 }
    }

    transitions: Transition {
        PropertyAnimation { duration: 3000 }
        ColorAnimation { duration: 3000 }
    }

    //TODO: Below feature needs beautification to meet minimum standards
    // toolbox
    Rectangle {
        id: toolbox
        z: 3 //Above ground
        color: "white"
        width: 480
        anchors { right: parent.right; top:parent.top; bottom: parent.bottom }
        Column{
            id: toolboxPositioner
            anchors.centerIn: parent
            spacing: 4
            Text{ text: "Drag an item into the scene." }
            Row{ spacing: 4; 
                height: childrenRect.height//TODO: Put bug in JIRA when it comes back up
                PaletteItem{ 
                    anchors.verticalCenter: parent.verticalCenter
                    file: "Sun.qml";
                    image: "images/sun.png"
                }
                PaletteItem{ 
                    anchors.verticalCenter: parent.verticalCenter
                    file: "GenericItem.qml"
                    image: "images/moon.png"
                }
                PaletteItem{ 
                    anchors.verticalCenter: parent.verticalCenter
                    file: "GenericItem.qml"
                    image: "images/tree_s.png"
                }
                PaletteItem{ 
                    anchors.verticalCenter: parent.verticalCenter
                    file: "GenericItem.qml"
                    image: "images/rabbit_brown.png"
                }
                PaletteItem{ 
                    anchors.verticalCenter: parent.verticalCenter
                    file: "GenericItem.qml"
                    image: "images/rabbit_bw.png"
                }
            }
            Text{ text: "Active Suns: " + activeSuns }
            Rectangle { width: 440; height: 1; color: "black" }
            Text{ text: "Arbitrary QML: " }
            TextEdit {
                id: qmlText
                width: 460
                height: 220
                readOnly: false
                focusOnPress: true
                font.pixelSize: 16
                
                text: "import Qt 4.6\nImage { id: smile;\n  x: 500*Math.random();\n  y: 250*Math.random(); \n  source: 'images/face-smile.png';\n  opacity: NumberAnimation{ \n    running:true; to: 0; duration: 1500;\n  }\n   Component.onCompleted: smile.destroy(1500);\n}"
            }
            Rectangle {
                width: 80
                height: 20
                color: "lightsteelblue"
                Text{ anchors.centerIn: parent; text: "Create" }
                MouseRegion { 
                    anchors.fill: parent; 
                    onClicked: {var obj=createQmlObject(qmlText.text, window, 'CustomObject'); obj.parent=window;}
                }
            }
        }
    }

}
