import Qt 4.6

//! [0]
Rect { width: 220; height: 20; color: "white"
    Component { id: Dot
        Rect { width: 20; height: 20; radius: 10; color: "green" }
    }
    HorizontalLayout {
        Rect { width: 10; height: 20; color: "red" }
        Repeater { component: Dot; dataSource: 10 }
        Rect { width: 10; height: 20; color: "blue" }
    }
}
//! [0]
