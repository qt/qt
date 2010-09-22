#include "private/qdeclarativedebughelper_p.h"
#include "private/qdeclarativeengine_p.h"

#include <QtScript/QScriptEngine>

QT_BEGIN_NAMESPACE

QScriptEngine *QDeclarativeDebugHelper::getScriptEngine(QDeclarativeEngine *engine)
{
    return QDeclarativeEnginePrivate::getScriptEngine(engine);
}

QT_END_NAMESPACE
