//![0]
import Qt 4.7

Item { 
    id: block

    property int type: 0
    
    Image {
        id: img

        anchors.fill: parent
        source: {
            if (type == 0)
                return "../shared/pics/redStone.png";
            else if (type == 1) 
                return "../shared/pics/blueStone.png";
            else
                return "../shared/pics/greenStone.png";
        }
    }
}
//![0]
