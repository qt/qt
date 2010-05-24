import Qt 4.7
import Effects 1.0

Item {
    id: main
    property alias source: loader.source
    property bool show: false
    x: 0
    y: -500 //height and width set by program
    opacity: 0
    property QtObject blurEffect: realBlur == null ? dummyBlur : realBlur //Is there a better way to lose those error messages?
    Loader{//Automatic FocusScope
        focus: true
        clip: true
        id: loader //source set by program
        anchors.centerIn: parent
        onStatusChanged: if(status == Loader.Ready) {
                if(loader.item.width > 640)
                    loader.item.width = 640;
                if(loader.item.height > 480)
                    loader.item.height = 480;
       }

    }
    Rectangle{
        z: -1
        anchors.fill: loader.status == Loader.Ready ? loader : errorTxt
        anchors.margins: -10
        radius: 12
        smooth: true
        gradient: Gradient{
            GradientStop{ position: 0.0; color: "#14FFFFFF" }
            GradientStop{ position: 1.0; color: "#5AFFFFFF" }
        }
        MouseArea{
            anchors.fill: parent
            onClicked: loader.focus=true;/* and don't propogate to the 'exit' area*/
        }

    }

    MouseArea{
        z: -2
        hoverEnabled: true //To steal from the buttons
        anchors.fill: parent
        onClicked: main.show=false;
    }

    Text{
        id: errorTxt
        anchors.centerIn: parent
        color: 'white'
        smooth: true
        visible: loader.status == Loader.Error
        textFormat: Text.RichText //includes link for bugreport
        //Note that if loader is Error, it is because the file was found but there was an error creating the component
        //This means either we have a bug in our demos, or the required modules (which ship with Qt) did not deploy correctly
        text: 'The example has failed to load. This is a bug!<br />'
            +'Report it at <a href="http://bugreports.qt.nokia.com">http://bugreports.qt.nokia.com</a>';
        onLinkActivated: Qt.openUrlExternally(link);
    }


    states: [
        State {
            name: "show"
            when: show == true
            PropertyChanges {
                target: main
                opacity: 1
                y: 0                
            }
            PropertyChanges {
                target: blurEffect
                enabled: true
                blurRadius: 8
                blurHints: Blur.AnimationHint | Blur.PerformanceHint
            }
        }
    ]
    MagicAnim{ id: magicAnim; target: main; from: -500; to: 0 }
    transitions: [
        Transition { from: ""; to: "show"
            SequentialAnimation{
                ScriptAction{ script: magicAnim.start() }
                NumberAnimation{ properties: "opacity,blurRadius";  easing.type: Easing.OutCubic; duration: 1000}
                PropertyAnimation{ target: main; property: "y"}
            }

        },
        Transition { from: "show"; to: "" //Addtionally, unload the item
            SequentialAnimation{
                NumberAnimation{ properties: "y,opacity,blurRadius";duration: 500 }
                ScriptAction{ script: loader.source = ''; }
            }
            /*Attempt to copy the exeunt animation. Looks bad
            SequentialAnimation{
                ParallelAnimation{
                    NumberAnimation{ properties: "opacity,blurRadius"; easing.type: Easing.InCubic; duration: 1000 }
                    SequentialAnimation{
                        NumberAnimation{ target: main; property: 'y'; to: 3.2*height/5; duration: 500}
                        ParallelAnimation{
                            NumberAnimation{ target: main; property: 'y'; to: 3.0*height/5; duration: 100}
                            NumberAnimation{ target: main; property: 'x'; to: 3.0*width/5; duration: 100}
                        }
                        NumberAnimation{ target: main; property: 'x'; to: 700; duration: 400}
                    }
                }
                ScriptAction{ script: loader.source = ''; }
                PropertyAction{ properties: "x,y";}
            }
            */
        }
    ]
    Item{ Blur{id: dummyBlur } }

}
