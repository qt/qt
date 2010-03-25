import Qt 4.6
import QmlTime 1.0 as QmlTime

Item {

    QmlTime.Timer {
        component: Component {
            Item {
                Loader {
                    sourceComponent: Loaded {}
                }
            }
        }
    }
}

