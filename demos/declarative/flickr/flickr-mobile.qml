import Qt 4.6
import "common" as Common
import "mobile" as Mobile

Item {
    id: Screen; width: 320; height: 480
    property bool inListView : false

    Rectangle {
        id: Background
        anchors.fill: parent; color: "#343434";

        Image { source: "mobile/images/stripes.png"; fillMode: "Tile"; anchors.fill: parent; opacity: 0.3 }

        Common.RssModel { id: RssModel }
        Common.Loading { anchors.centerIn: parent; visible: RssModel.status }

        Item {
            id: Views
            x: 2; width: parent.width - 4
            anchors.top: TitleBar.bottom; anchors.bottom: ToolBar.top

            Mobile.GridDelegate { id: GridDelegate }
            GridView {
                id: PhotoGridView; model: RssModel; delegate: GridDelegate; cacheBuffer: 100
                cellWidth: 79; cellHeight: 79; width: parent.width; height: parent.height - 1
            }

            Mobile.ListDelegate { id: ListDelegate }
            ListView {
                id: PhotoListView; model: RssModel; delegate: ListDelegate
                width: parent.width; height: parent.height; x: -(parent.width * 1.5); cacheBuffer: 100;
            }
        }

        Common.ImageDetails { id: ImageDetails; width: parent.width; x: parent.width; height: parent.height }
        Mobile.TitleBar { id: TitleBar; width: parent.width; height: 40; opacity: 0.9 }
        Mobile.ToolBar { id: ToolBar; height: 40; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9 }

        states: [
            State {
                name: "ListView"; when: Screen.inListView == true
                PropertyChanges { target: PhotoListView; x: 0 }
                PropertyChanges { target: PhotoGridView; x: -(parent.width * 1.5) }
            }
        ]
        transitions: [
            Transition { NumberAnimation { properties: "x"; duration: 500; easing: "easeInOutQuad" } }
        ]
    }
}
