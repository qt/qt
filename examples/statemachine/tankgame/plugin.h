#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtPlugin>

class QState;
class Plugin
{
public:
    virtual ~Plugin() {}

    virtual QState *create(QState *parentState, QObject *tank) = 0;
};

Q_DECLARE_INTERFACE(Plugin, "TankPlugin")

#endif
