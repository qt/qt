import Qt 4.6

GenericItem {
    id: itemButton
    property string file
    Script { source: "itemCreation.js" }
    MouseRegion { 
        anchors.fill: parent; 
        onPressed: startDrag(mouse);
        onPositionChanged: moveDrag(mouse);
        onReleased: endDrag(mouse);
    }
}
