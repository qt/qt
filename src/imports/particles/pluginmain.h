#ifndef PLUGINMAIN_H
#define PLUGINMAIN_H

#include <QtDeclarative>
#include <QtDeclarative/QDeclarativeExtensionPlugin>


class ParticlesPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri);
};

#endif // PLUGINMAIN_H
