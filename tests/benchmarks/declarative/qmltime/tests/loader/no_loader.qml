import Qt 4.7
import QmlTime 1.0 as QmlTime

Item {

    QmlTime.Timer {
        component: Component {
            Item {
                Loaded {}
            }
        }
    }
}

