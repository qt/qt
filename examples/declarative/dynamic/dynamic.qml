import Qt 4.6

Item {
    id: window
    //This is a desktop example
    width: 1024; height: 480
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
            spacing: 1
            Sun {
                id: sunButton
                Script { source: "sunCreation.js" }
                MouseRegion { 
                    anchors.fill: parent; 
                    onPressed: startDrag(mouse);
                    onPositionChanged: moveDrag(mouse);
                    onReleased: endDrag(mouse);
                }
            }
            Text{ text: "Active Suns: " + activeSuns }
            Rectangle { width: 440; height: 1; color: "black" }
            Text{ text: "Arbitrary Javascript: " }
            TextEdit {
                id: jsText
                width: 460
                height: 80
                readOnly: false
                focusOnPress: true
                
                text: "window.activeSuns++;"
            }
            Rectangle {
                width: 80
                height: 20
                color: "lightsteelblue"
                Text{ anchors.centerIn: parent; text: "Execute" }
                MouseRegion { 
                    anchors.fill: parent; 
                    onClicked: eval(jsText.text.toString()); 
                }
            }

            Rectangle { width: 440; height: 1; color: "black" }
            Text{ text: "Arbitrary QML: " }
            TextEdit {
                id: qmlText
                width: 460
                height: 180
                readOnly: false
                focusOnPress: true
                
                text: "import Qt 4.6\nImage { id: smile; x: 10; y: 10; \n  source: 'images/face-smile.png';\n  opacity: NumberAnimation{ \n    running:true; to: 0; duration: 1500;\n  }\n   Component.onCompleted: smile.destroy(1500);\n}"
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
