import Qt 4.6
import QDeclarativeTime 1.0 as QDeclarativeTime

Item {

    property string name: "Bob Smith"

    QDeclarativeTime.Timer {
        component: Item {
            Text { text: name }
        }
    }
}

