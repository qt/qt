import Qt 4.6

FocusScope {
    clip: true

    onWantsFocusChanged: if (wantsFocus) mainView.state = "showListViews"

    ListView {
        id: list1
        delegate: ListViewDelegate {}
        y: wantsFocus ? 10 : 40; focus: true; width: parent.width / 3; height: parent.height - 20
        model: 10; KeyNavigation.up: gridMenu; KeyNavigation.left: contextMenu; KeyNavigation.right: list2
        Behavior on y { NumberAnimation { duration: 600; easing.type: "OutQuint" } }
    }

    ListView {
        id: list2
        delegate: ListViewDelegate {}
        y: wantsFocus ? 10 : 40; x: parent.width / 3; width: parent.width / 3; height: parent.height - 20
        model: 10; KeyNavigation.up: gridMenu; KeyNavigation.left: list1; KeyNavigation.right: list3
        Behavior on y { NumberAnimation { duration: 600; easing.type: "OutQuint" } }
    }

    ListView {
        id: list3
        delegate: ListViewDelegate {}
        y: wantsFocus ? 10 : 40; x: 2 * parent.width / 3; width: parent.width / 3; height: parent.height - 20
        model: 10; KeyNavigation.up: gridMenu; KeyNavigation.left: list2
        Behavior on y { NumberAnimation { duration: 600; easing.type: "OutQuint" } }
    }

    Rectangle { width: parent.width; height: 1; color: "#D1DBBD" }
    Rectangle {
        y: 1; width: parent.width; height: 10
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#3E606F" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
    Rectangle {
        y: parent.height - 10; width: parent.width; height: 10
        gradient: Gradient {
            GradientStop { position: 1.0; color: "#3E606F" }
            GradientStop { position: 0.0; color: "transparent" }
        }
    }
}
