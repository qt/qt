#ifndef QMLINSPECTORRUNCONTROL_H
#define QMLINSPECTORRUNCONTROL_H

#include <QtCore/qobject.h>

#include <projectexplorer/runconfiguration.h>

#include "qmlinspector.h"

namespace ProjectExplorer {
    class ApplicationLauncher;
}

class QmlInspectorRunControlFactory : public ProjectExplorer::IRunControlFactory
{
    Q_OBJECT

public:
    explicit QmlInspectorRunControlFactory(QObject *parent);

    virtual bool canRun(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode) const;

    virtual ProjectExplorer::RunControl *create(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode);

    ProjectExplorer::RunControl *create(
                const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                const QString &mode,
                const QmlInspector::StartParameters &sp);
                
    virtual QString displayName() const;

    virtual QWidget *configurationWidget(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration);
};

class QmlInspectorRunControl : public ProjectExplorer::RunControl
{
    Q_OBJECT

public:
    explicit QmlInspectorRunControl(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
                                    const QmlInspector::StartParameters &sp = QmlInspector::StartParameters());
    ~QmlInspectorRunControl();
    
    virtual void start();
    virtual void stop();
    virtual bool isRunning() const;

private slots:
    void appendOutput(const QString &s);
    void appStarted();
    void delayedStart();
    void viewerExited();
    void applicationError(const QString &error);

private:
    bool m_running;
    ProjectExplorer::ApplicationLauncher *m_viewerLauncher;
    QmlInspector::StartParameters m_startParams;
};

#endif
