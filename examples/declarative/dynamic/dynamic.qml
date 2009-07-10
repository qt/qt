Rect { id: page; width: 800; height: 800; color:"black"
    Script { source: "dynamic.js" }
    property bool extendStars: false;
    Item { id: targetItem; x: 100; y: 100; }
    Item { id: targetItem2; x: 0; y: 300; }
    Rect { width: 100; height: 100; color: "green"; id: rect
        MouseRegion { anchors.fill:parent; onClicked: {a = createWithComponent();}}
    }
    Rect { width: 100; height: 100; color: "red"; id: rect2; y:100;
        MouseRegion { anchors.fill:parent; onClicked: {destroyDynamicObject();}}
    }
    Rect { width: 100; height: 100; color: "blue"; id: rect3; y:200;
        MouseRegion { anchors.fill:parent; onClicked: 
            { 
                if(fourthBox == null) {
                    a = createQml(targetItem2);
                    if(a!=null) {
                        a.parent = targetItem2;//BUG: this should happen automatically
                        fourthBox = a;
                        extendStars = true;
                    }
                } else {
                    fourthBox.destroy();
                    fourthBox = null;
                    extendStars = false;
                }
            }
        }
    }
    Particles { x:0; y:0; count:20; lifeSpan:500; width:100; height: if(extendStars){400;}else{300;} source:"star.png"}
}
