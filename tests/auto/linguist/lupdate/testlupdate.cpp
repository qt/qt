#include "testlupdate.h"
#include <stdlib.h>
#include <QtGui/QApplication>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QDir>

#ifdef Q_OS_WIN32
#   include <windows.h>
#endif

#include <QtTest/QtTest>


TestLUpdate::TestLUpdate()
{
    childProc = 0;
    m_cmdLupdate = QLatin1String("lupdate");
    m_cmdQMake = QLatin1String("qmake");
}

TestLUpdate::~TestLUpdate()
{
    if (childProc)
        delete childProc;
}

void TestLUpdate::setWorkingDirectory(const QString &workDir)
{
    m_workDir = workDir;
    QDir::setCurrent(m_workDir);
}

void TestLUpdate::addMakeResult( const QString &result )
{
    make_result.append( result );
}

bool TestLUpdate::runChild( bool showOutput, const QString &program, const QStringList &argList)
{
    make_result.clear();
    exit_ok = FALSE;
    if (childProc)
        delete childProc;

    child_show = showOutput;
    if ( showOutput ) {
        QString S = argList.join(" ");
        addMakeResult( program + QLatin1String(" ") + S );
    }

    childProc = new QProcess();
    Q_ASSERT(childProc);

    childProc->setWorkingDirectory(m_workDir);
    connect(childProc, SIGNAL(finished(int)), this, SLOT(childReady(int)));
    childProc->setProcessChannelMode(QProcess::MergedChannels);
    if (argList.isEmpty()) {
        childProc->start( program );
    } else {
        childProc->start( program, argList );
    }
    bool ok;

    ok = childProc->waitForStarted();

    if (ok)
        ok = childProc->waitForFinished();

    if (!ok)
        addMakeResult( "Error executing '" + program + "'." );

    childReady(ok ? 0 : -1);

    return ok;
}

void TestLUpdate::childReady(int /*exitCode*/)
{
    if (childProc != 0) {
        childHasData();
        exit_ok = childProc->state() == QProcess::NotRunning
                  && childProc->exitStatus() == 0;
        childProc->deleteLater();
    }
    childProc = 0;
}

void TestLUpdate::childHasData()
{
    //QByteArray ba = childProc->readAllStandardError();
    //qDebug() << "ERROR:" << ba;
    QString stdoutput = childProc->readAllStandardOutput();
    stdoutput = stdoutput.replace("\t", "    ");
    if (child_show)
        addMakeResult(stdoutput);
}

bool TestLUpdate::run(const QString &commandline)
{
    return runChild(true, m_cmdLupdate + QLatin1String(" ") + commandline);
}


bool TestLUpdate::updateProFile(const QString &arguments)
{
    QStringList args = arguments.split(QChar(' '));
    return runChild( true, m_cmdLupdate, args );
}

bool TestLUpdate::qmake()
{
    QStringList args;
    args << "-r";
    return runChild(true, m_cmdQMake, args);
}
