import Qt 4.7
import "Core"

Rectangle {
    id: window
    
    width: 800; height: 480
    color: "#3E606F"

    FocusScope {
        id: mainView

        width: parent.width; height: parent.height
        focus: true

        GridMenu {
            id: gridMenu

            width: parent.width; height: 320
            focus: true
            interactive: parent.wantsFocus
        }

        ListViews {
            id: listViews
            y: 320; width: parent.width; height: 320
        }

        Rectangle { 
            id: shade
            color: "black"; opacity: 0; anchors.fill: parent
        }

        states: State {
            name: "showListViews"
            PropertyChanges { target: gridMenu; y: -160 }
            PropertyChanges { target: listViews; y: 160 }
        }

        transitions: Transition {
            NumberAnimation { properties: "y"; duration: 600; easing.type: "OutQuint" }
        }
    }

    Image {
        source: "Core/images/arrow.png"
        rotation: 90
        anchors.verticalCenter: parent.verticalCenter

        MouseArea {
            anchors { fill: parent; leftMargin: -10; topMargin: -10; rightMargin: -10; bottomMargin: -10 }
            onClicked: window.state = "contextMenuOpen"
        }
    }

    ContextMenu { id: contextMenu; x: -265; width: 260; height: parent.height }

    states: State {
        name: "contextMenuOpen"
        when: !mainView.wantsFocus
        PropertyChanges { target: contextMenu; x: 0; open: true }
        PropertyChanges { target: mainView; x: 130 }
        PropertyChanges { target: shade; opacity: 0.25 }
    }

    transitions: Transition {
        NumberAnimation { properties: "x,opacity"; duration: 600; easing.type: "OutQuint" }
    }
}
