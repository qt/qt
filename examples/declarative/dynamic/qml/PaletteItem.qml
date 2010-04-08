import Qt 4.7
import "itemCreation.js" as Code

GenericItem {
    id: itemButton
    property string file
    MouseArea { 
        anchors.fill: parent; 
        onPressed: Code.startDrag(mouse);
        onPositionChanged: Code.moveDrag(mouse);
        onReleased: Code.endDrag(mouse);
    }
}
