V8DIR = $$(V8DIR)
isEmpty(V8DIR) {
    V8DIR = $$PWD/../3rdparty/v8
} else {
    message(using external V8 from $$V8DIR)
}

QMAKE_CXXFLAGS += -Wno-unused-parameter
