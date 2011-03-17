V8DIR = $$(V8DIR)
isEmpty(V8DIR) {
    V8DIR = $$PWD/../../3rdparty/v8
} else {
    message(using external V8 from $$V8DIR)
}

*-g++*: {
    QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}
