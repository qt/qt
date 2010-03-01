import Qt 4.6

Rectangle {
    width: 500; height: 250; color: "#edecec"

    Column {
        anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter; spacing: 10

        SearchBox { id: search1; KeyNavigation.down: search2; focus: true }
        SearchBox { id: search2; KeyNavigation.up: search1; KeyNavigation.down: search3 }
        SearchBox { id: search3; KeyNavigation.up: search2 }
    }
}
