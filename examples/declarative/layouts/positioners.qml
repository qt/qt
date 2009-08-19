import Qt 4.6
Rect { width: 420; height: 420; id:page; color:"white"
    VerticalPositioner { id: layout1; y:0; //width: 100; height:250;
        move: Transition{ NumberAnimation {properties: "y"; easing: "easeOutBounce" }}
        add: Transition{ NumberAnimation { properties: "y"; from: 500; duration:500; easing: "easeOutQuad"}}
        remove: Transition { NumberAnimation { properties:"y"; to: 500; duration:500; easing: "easeInQuad"}}
        Rect { color: "red"; width: 100; height: 50; border.color: "black"; radius: 15 }
        Rect { id: blueV1; color: "lightsteelblue"; width: 100; height: 50; border.color: "black"; radius: 15 
            //opacity: Behavior{ NumberAnimation {}}
        }
        Rect { color: "green"; width: 100; height: 50; border.color: "black"; radius: 15 }
        Rect { id: blueV2; color: "lightsteelblue"; width: 100; height: 50; border.color: "black"; radius: 15 
            //opacity: Behavior{ NumberAnimation {}}
        }
        Rect { color: "orange"; width: 100; height: 50; border.color: "black"; radius: 15 }
    }
    HorizontalPositioner { id: layout2; y:300; 
        move: Transition{ NumberAnimation {properties: "x"; easing: "easeOutBounce" }}
        add: Transition{ NumberAnimation {  properties: "x"; from: 500; duration:500; easing: "easeOutQuad"}
                    NumberAnimation {  properties: "opacity"; from: 0;  duration: 500;}}
        remove: Transition { NumberAnimation { properties: "x"; to: 500; duration:500; easing: "easeInQuad"}
                    NumberAnimation {  properties: "opacity"; from: 1; duration: 500}}
        Rect { color: "red"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { id: blueH1; color: "lightsteelblue"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "green"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { id: blueH2; color: "lightsteelblue"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "orange"; width: 50; height: 100; border.color: "black"; radius: 15 }
    }
    Button { text: "Remove"; icon: "del.png"; x: 135; y:90;
        onClicked: {blueH2.opacity=0; blueH1.opacity=0; blueV1.opacity=0; blueV2.opacity=0; blueG1.opacity=0; blueG2.opacity=0; blueG3.opacity=0;}
        }
    Button { text: "Add"; icon: "add.png"; x: 145; y:140;
        onClicked: {blueH2.opacity=1; blueH1.opacity=1; blueV1.opacity=1; blueV2.opacity=1; blueG1.opacity=1; blueG2.opacity=1; blueG3.opacity=1;}
        }
    GridPositioner { x:260; y:0; columns:3
        remove: Transition { NumberAnimation{ properties: "opacity"; from: 1; to: 0; duration: 500}
           NumberAnimation{properties: "x,y"; easing: "easeOutBounce"} }
        move: Transition { NumberAnimation{ properties: "x,y"; easing: "easeOutBounce" }}
        add: Transition { NumberAnimation{ properties: "opacity"; from: 0; to: 1; duration: 500}
           NumberAnimation{properties: "x,y"; easing: "easeOutBounce"} }
        Rect { color: "red"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { id: blueG1; color: "lightsteelblue"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "green"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { id: blueG2; color: "lightsteelblue"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "orange"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { id: blueG3; color: "lightsteelblue"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "red"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "green"; width: 50; height: 100; border.color: "black"; radius: 15 }
        Rect { color: "orange"; width: 50; height: 100; border.color: "black"; radius: 15 }
    }
}
