HEADERS += $$PWD/qstatemachine.h \
	   $$PWD/qstatemachine_p.h \
	   $$PWD/qstateaction.h \
	   $$PWD/qstateaction_p.h \
	   $$PWD/qsignaleventgenerator_p.h \
	   $$PWD/qabstractstate.h \
	   $$PWD/qabstractstate_p.h \
	   $$PWD/qactionstate.h \
	   $$PWD/qactionstate_p.h \
	   $$PWD/qstate.h \
	   $$PWD/qstate_p.h \
	   $$PWD/qfinalstate.h \
	   $$PWD/qhistorystate.h \
	   $$PWD/qhistorystate_p.h \
	   $$PWD/qabstracttransition.h \
	   $$PWD/qabstracttransition_p.h \
	   $$PWD/qtransition.h \
	   $$PWD/qtransition_p.h \
	   $$PWD/qstatefinishedevent.h \
	   $$PWD/qstatefinishedtransition.h \
	   $$PWD/qsignalevent.h \
	   $$PWD/qsignaltransition.h \
	   $$PWD/qsignaltransition_p.h

SOURCES += $$PWD/qstatemachine.cpp \
	   $$PWD/qstateaction.cpp \
	   $$PWD/qabstractstate.cpp \
	   $$PWD/qactionstate.cpp \
	   $$PWD/qstate.cpp \
	   $$PWD/qfinalstate.cpp \
	   $$PWD/qhistorystate.cpp \
	   $$PWD/qabstracttransition.cpp \
	   $$PWD/qtransition.cpp \
	   $$PWD/qstatefinishedtransition.cpp \
	   $$PWD/qsignaltransition.cpp

!contains(DEFINES, QT_NO_STATEMACHINE_EVENTFILTER) {
HEADERS += $$PWD/qboundevent_p.h \
	   $$PWD/qeventtransition.h \
	   $$PWD/qeventtransition_p.h
SOURCES += $$PWD/qeventtransition.cpp
}

!contains(DEFINES, QT_NO_ANIMATION) {
HEADERS += $$PWD/qanimationstate.h
SOURCES += $$PWD/qanimationstate.cpp
}
