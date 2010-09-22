#ifndef QDECLARATIVEDEBUGHELPER_P_H
#define QDECLARATIVEDEBUGHELPER_P_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QScriptEngine;
class QDeclarativeEngine;

// Helper methods to access private API through a stable interface
// This is used in the qmljsdebugger library of QtCreator.
class Q_DECLARATIVE_EXPORT QDeclarativeDebugHelper
{
public:
    static QScriptEngine *getScriptEngine(QDeclarativeEngine *engine);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEDEBUGHELPER_P_H
