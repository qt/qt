#ifndef PLUGINMAIN_H
#define PLUGINMAIN_H

#include <QtDeclarative>
#include <QtDeclarative/QDeclarativeExtensionPlugin>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticlesPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri);
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // PLUGINMAIN_H
