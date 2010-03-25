import Qt 4.6
import QDeclarativeTime 1.0 as QDeclarativeTime

Item {

    QDeclarativeTime.Timer {
        component: Component {
            Item {
                Loader {
                    source: "Loaded.qml"
                }
            }
        }
    }
}

