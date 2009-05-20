#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QState;
QT_END_NAMESPACE
class Plugin
{
public:
    virtual ~Plugin() {}

    virtual QState *create(QState *parentState, QObject *tank) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Plugin, "TankPlugin")
QT_END_NAMESPACE

#endif
