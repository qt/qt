#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>

#include <projectexplorer/applicationlauncher.h>
#include <projectexplorer/applicationrunconfiguration.h>
#include <projectexplorer/projectexplorerconstants.h>

#include "runcontrol.h"

using namespace ProjectExplorer;


QmlInspectorRunControlFactory::QmlInspectorRunControlFactory(QObject *parent)
    : ProjectExplorer::IRunControlFactory(parent)
{
}

bool QmlInspectorRunControlFactory::canRun(const QSharedPointer<RunConfiguration> &runConfiguration, const QString &mode) const
{
    Q_UNUSED(runConfiguration);
    if (mode != ProjectExplorer::Constants::RUNMODE)
        return false;
    return true;
}

ProjectExplorer::RunControl *QmlInspectorRunControlFactory::create(const QSharedPointer<RunConfiguration> &runConfiguration, const QString &mode)
{
    Q_UNUSED(mode);
    return new QmlInspectorRunControl(runConfiguration);
}

ProjectExplorer::RunControl *QmlInspectorRunControlFactory::create(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
const QString &mode, const QmlInspector::StartParameters &sp)
{
    Q_UNUSED(mode);
    return new QmlInspectorRunControl(runConfiguration, sp);
}
                
QString QmlInspectorRunControlFactory::displayName() const
{
    return tr("Qml Inspector");
}

QWidget *QmlInspectorRunControlFactory::configurationWidget(const QSharedPointer<RunConfiguration> &runConfiguration)
{
    Q_UNUSED(runConfiguration);
    return 0;
}



QmlInspectorRunControl::QmlInspectorRunControl(const QSharedPointer<ProjectExplorer::RunConfiguration> &runConfiguration,
const QmlInspector::StartParameters &sp)
    : ProjectExplorer::RunControl(runConfiguration),
      m_running(false),
      m_viewerLauncher(0),
      m_startParams(sp)
{
}

QmlInspectorRunControl::~QmlInspectorRunControl()
{
}

void QmlInspectorRunControl::start()
{
    if (m_running || m_viewerLauncher)
        return;

    m_viewerLauncher = new ProjectExplorer::ApplicationLauncher(this);
    connect(m_viewerLauncher, SIGNAL(applicationError(QString)), SLOT(applicationError(QString)));
    connect(m_viewerLauncher, SIGNAL(processExited(int)), SLOT(viewerExited()));
    connect(m_viewerLauncher, SIGNAL(appendOutput(QString)), SLOT(appendOutput(QString)));
    connect(m_viewerLauncher, SIGNAL(bringToForegroundRequested(qint64)),
            this, SLOT(appStarted()));

    QSharedPointer<LocalApplicationRunConfiguration> rc =
        runConfiguration().objectCast<LocalApplicationRunConfiguration>();
    if (rc.isNull()) {  // TODO
        return;
    }

    ProjectExplorer::Environment env = rc->environment();
    env.set("QML_DEBUG_SERVER_PORT", QString::number(m_startParams.port));

    QStringList arguments = rc->commandLineArguments();
    arguments << QLatin1String("-stayontop");

    m_viewerLauncher->setEnvironment(env.toStringList());
    m_viewerLauncher->setWorkingDirectory(rc->workingDirectory());

    m_running = true;

    m_viewerLauncher->start(static_cast<ApplicationLauncher::Mode>(rc->runMode()),
                            rc->executable(), arguments);
}

void QmlInspectorRunControl::stop()
{
    if (m_viewerLauncher->isRunning())
        m_viewerLauncher->stop();
}

bool QmlInspectorRunControl::isRunning() const
{
    return m_running;
}

void QmlInspectorRunControl::appStarted()
{
    QTimer::singleShot(500, this, SLOT(delayedStart()));
}

void QmlInspectorRunControl::appendOutput(const QString &s)
{
    emit addToOutputWindow(this, s);
}

void QmlInspectorRunControl::delayedStart()
{
    emit started();
}

void QmlInspectorRunControl::viewerExited()
{
    m_running = false;
    emit finished();
    
    deleteLater();
}

void QmlInspectorRunControl::applicationError(const QString &s)
{
    emit error(this, s);
}
