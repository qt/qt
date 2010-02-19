#ifndef QMLEXTENSIONPLUGIN_H
#define QMLEXTENSIONPLUGIN_H

#include <QtCore/qplugin.h>

#include "qmlextensioninterface.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlEngine;

class Q_DECLARATIVE_EXPORT QmlExtensionPlugin : public QObject, public QmlExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(QmlExtensionInterface)
public:
    explicit QmlExtensionPlugin(QObject *parent = 0);
    ~QmlExtensionPlugin();

    virtual void initialize(QmlEngine *engine) = 0;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLEXTENSIONPLUGIN_H
