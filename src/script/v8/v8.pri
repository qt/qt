include($$PWD/v8base.pri)

V8_GENERATED_SOURCES_DIR = generated

DEFINES += ENABLE_DEBUGGER_SUPPORT

# Because our patches to V8 are guarded by this define.
DEFINES += QT_BUILD_SCRIPT_LIB

CONFIG(debug, debug|release) {
    DEFINES += DEBUG ENABLE_VMSTATE_TRACKING ENABLE_LOGGING_AND_PROFILING V8_ENABLE_CHECKS
} else {
    DEFINES += NDEBUG
}

INCLUDEPATH += \
    $$V8DIR/src \
    $$V8DIR/include
