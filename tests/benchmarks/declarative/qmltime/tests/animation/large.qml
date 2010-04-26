import Qt 4.7
import QmlTime 1.0 as QmlTime

Item {

    QmlTime.Timer {
        component: Component {
            ParallelAnimation {
                NumberAnimation { duration: 500 }
                NumberAnimation { duration: 4000; }
                NumberAnimation { duration: 2000; easing.type: "OutBack"}
                ColorAnimation { duration: 3000}
                SequentialAnimation {
                    PauseAnimation { duration: 1000 }
                    ScriptAction { script: doSomething(); }
                    PauseAnimation { duration: 800 }
                    ScriptAction { script: doSomethingElse(); }
                    PauseAnimation { duration: 800 }
                    ParallelAnimation {
                        NumberAnimation { duration: 200;}
                        SequentialAnimation {
                            PauseAnimation { duration: 200}
                            ParallelAnimation {
                                NumberAnimation { duration: 300;}
                                NumberAnimation { duration: 300;}
                            }
                            NumberAnimation { from: 0; to: 1; duration: 500 }
                            PauseAnimation { duration: 200 }
                            NumberAnimation { from: 1; to: 0; duration: 500 }
                        }
                        SequentialAnimation {
                            PauseAnimation { duration: 150}
                            NumberAnimation { duration: 300; easing.type: "OutBounce" }
                        }
                    }
                }
            }
        }
    }

}
