import Qt 4.6

Rect { 
    id: page; color: activePalette.button; width: txtItem.width+20; height: txtItem.height+4
    pen.width: 1; pen.color: activePalette.mid; radius: 10;
    signal clicked
    property string text: "Button"
    gradient: Gradient { 
        GradientStop { id:topGrad; position: 0.0; 
            color: if(mr.pressed){activePalette.dark;}else{activePalette.light;}}
        GradientStop { position: 1.0; color: activePalette.button }
    }
    MouseRegion { id:mr; anchors.fill: parent; onClicked: page.clicked() }
    Text {
        id: txtItem; text: page.text; anchors.centeredIn: page; color: activePalette.buttonText 
        font.size: 14;
    }
}
