import Qt 4.6
import QDeclarativeTime 1.0 as QDeclarativeTime

Item {

    QDeclarativeTime.Timer {
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
