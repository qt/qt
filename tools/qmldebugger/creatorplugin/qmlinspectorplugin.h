#ifndef QMLINSPECTORPLUGIN_H
#define QMLINSPECTORPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QStringList;


class QmlInspectorRunControlFactory;
class QmlInspectorMode;
class InspectorOutputPane;

namespace ProjectExplorer
{
    class RunControl;
}

class QmlInspectorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    QmlInspectorPlugin();
    ~QmlInspectorPlugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();
    virtual void shutdown();

private slots:
    void startViewer();
    void stopViewer();

private:
    QmlInspectorMode *m_inspectMode;
    InspectorOutputPane *m_outputPane;
    
    QmlInspectorRunControlFactory *m_runControlFactory;
    QPointer<ProjectExplorer::RunControl> m_runControl;
};


QT_END_NAMESPACE

#endif // QMLINSPECTORPLUGIN_H
