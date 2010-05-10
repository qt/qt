import Qt 4.7
import QmlTime 1.0 as QmlTime

Item {

    QmlTime.Timer {
        component: Component {
            ParallelAnimation {
                NumberAnimation { }
                NumberAnimation { }
                NumberAnimation { }
                ColorAnimation { }
                SequentialAnimation {
                    PauseAnimation { }
                    ScriptAction { }
                    PauseAnimation { }
                    ScriptAction { }
                    PauseAnimation { }
                    ParallelAnimation {
                        NumberAnimation { }
                        SequentialAnimation {
                            PauseAnimation { }
                            ParallelAnimation {
                                NumberAnimation { }
                                NumberAnimation { }
                            }
                            NumberAnimation { }
                            PauseAnimation { }
                            NumberAnimation { }
                        }
                        SequentialAnimation {
                            PauseAnimation { }
                            NumberAnimation { }
                        }
                    }
                }
            }
        }
    }

}
