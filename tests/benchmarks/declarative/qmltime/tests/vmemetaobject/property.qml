import Qt 4.7
import QmlTime 1.0 as QmlTime

Item {

    QmlTime.Timer {
        component: Component {
            QtObject {
                property string s
                property string s2
                property string s3
                property string s4
            }
        }
    }

}

