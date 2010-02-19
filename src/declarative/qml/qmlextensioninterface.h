#ifndef QMLEXTENSIONINTERFACE_H
#define QMLEXTENSIONINTERFACE_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlEngine;

struct Q_DECLARATIVE_EXPORT QmlExtensionInterface
{
    virtual void initialize(QmlEngine *engine) = 0;
};

Q_DECLARE_INTERFACE(QmlExtensionInterface, "com.trolltech.Qt.QmlExtensionInterface/1.0")

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLEXTENSIONINTERFACE_H
