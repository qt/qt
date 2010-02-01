import com.nokia.TimeExample 1.0 // import types from the plugin
import 'files' // import types from the 'files' directory

Clock { // this class is defined in QML (files/Clock.qml)

    Time { // this class is defined in C++ (plugins.cpp)
        id: time
    }

    hours: time.hour
    minutes: time.minute
}
