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
        Common.Loading { anchors.centerIn: parent; visible: RssModel.status == 2 }

        Item {
            id: Views
            x: 2; width: parent.width - 4
            anchors.top: TitleBar.bottom; anchors.bottom: ToolBar.top

            Mobile.GridDelegate { id: GridDelegate }
            GridView {
                id: PhotoGridView; model: RssModel; delegate: GridDelegate; cacheBuffer: 100
                cellWidth: 79; cellHeight: 79; width: parent.width; height: parent.height - 1; z: 6
            }

            Mobile.ListDelegate { id: ListDelegate }
            ListView {
                id: PhotoListView; model: RssModel; delegate: ListDelegate; z: 6
                width: parent.width; height: parent.height; x: -(parent.width * 1.5); cacheBuffer: 100;
            }
            states: State {
                name: "ListView"; when: Screen.inListView == true
                PropertyChanges { target: PhotoListView; x: 0 }
                PropertyChanges { target: PhotoGridView; x: -(parent.width * 1.5) }
            }

            transitions: Transition {
                NumberAnimation { properties: "x"; duration: 500; easing: "easeInOutQuad" }
            }
        }

        Mobile.ImageDetails { id: ImageDetails; width: parent.width; anchors.left: Views.right; height: parent.height; z:1 }
        Mobile.TitleBar { id: TitleBar; z: 5; width: parent.width; height: 40; opacity: 0.9 }

        Mobile.ToolBar {
            id: ToolBar; z: 5
            height: 40; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9
            button1Label: "Update"; button2Label: "View mode"
            onButton1Clicked: RssModel.reload()
            onButton2Clicked: if (Screen.inListView == true) Screen.inListView = false; else Screen.inListView = true
        }

        states: State {
            name: "DetailedView"
            PropertyChanges { target: Views; x: -parent.width }
            PropertyChanges { target: ToolBar; button1Label: "More..." }
            PropertyChanges { target: ToolBar; onButton1Clicked: { } }
            PropertyChanges { target: ToolBar; button2Label: "Back" }
            PropertyChanges { target: ToolBar; onButton2Clicked: { } }
        }

        transitions: Transition {
            NumberAnimation { properties: "x"; duration: 500; easing: "easeInOutQuad" }
        }
    }
}
