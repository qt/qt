import com.nokia.TimeExample 1.0 // import types from the plugin

Clock { // this class is defined in QML (files/Clock.qml)

    Time { // this class is defined in C++ (plugins.cpp)
        id: time
    }

    hours: time.hour
    minutes: time.minute
}
