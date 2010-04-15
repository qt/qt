import Qt 4.7

Rectangle {
    width: 500; height: 250
    color: "#edecec"

    Column {
        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
        spacing: 10

        SearchBox { id: search1; KeyNavigation.tab: search2; KeyNavigation.backtab: search3; focus: true }
        SearchBox { id: search2; KeyNavigation.tab: search3; KeyNavigation.backtab: search1 }
        SearchBox { id: search3; KeyNavigation.tab: search1; KeyNavigation.backtab: search2 }
    }
}
