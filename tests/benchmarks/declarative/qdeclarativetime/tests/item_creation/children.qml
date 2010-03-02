import Qt 4.6
import QDeclarativeTime 1.0 as QDeclarativeTime

Item {

    QDeclarativeTime.Timer {
        component: Component {
            Item {
                children: [
                    Rectangle { },
                    Rectangle { },
                    Item { },
                    Image { },
                    Text { },
                    Item { },
                    Item { },
                    Image { },
                    Image { },
                    Row { },
                    Image { },
                    Image { },
                    Column { },
                    Row { },
                    Text { },
                    Text { },
                    Text { },
                    MouseArea { }
                ]

            }
        }
    }

}
