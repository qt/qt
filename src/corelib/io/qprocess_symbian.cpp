/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QPROCESS_DEBUG

#ifdef QPROCESS_DEBUG
#include "qdebug.h"
#define QPROCESS_DEBUG_PRINT(args...) qDebug(args);
#else
#define QPROCESS_DEBUG_PRINT(args...)
#endif

#ifndef QT_NO_PROCESS

#define QPROCESS_ASSERT(check, panicReason, args...) \
    if (!(check)) { \
        qWarning(args); \
        User::Panic(KQProcessPanic, panicReason); \
    }

#include <exception>
#include <e32base.h>
#include <e32std.h>
#include <stdio.h>
#include "qplatformdefs.h"

#include "qstring.h"
#include "qprocess.h"
#include "qprocess_p.h"
#include "qeventdispatcher_symbian_p.h"

#include <private/qthread_p.h>
#include <qmutex.h>
#include <qmap.h>
#include <qsocketnotifier.h>

#include <errno.h>


QT_BEGIN_NAMESPACE

_LIT(KQProcessManagerThreadName, "QProcManThread");
_LIT(KQProcessPanic, "QPROCESS");
enum TQProcessPanic {
    EProcessManagerMediatorRunError         = 1,
    EProcessManagerMediatorInactive         = 2,
    EProcessManagerMediatorNotPending       = 3,
    EProcessManagerMediatorInvalidCmd       = 4,
    EProcessManagerMediatorCreationFailed   = 5,
    EProcessManagerMediatorThreadOpenFailed = 6,
    EProcessManagerMediatorNullObserver     = 7,
    EProcessActiveRunError                  = 10,
    EProcessActiveNullParameter             = 11,
    EProcessManagerMutexCreationFail        = 20,
    EProcessManagerThreadCreationFail       = 21,
    EProcessManagerSchedulerCreationFail    = 22,
    EProcessManagerNullParam                = 23
};

// Forward declarations
class QProcessManager;


// Active object to listen for child process death
class CProcessActive : public CActive
{
public:
    static CProcessActive* construct(QProcess* process,
                               RProcess** proc,
                               int serial,
                               int deathPipe);

    virtual ~CProcessActive();

    void start();
    void stop();

    bool error();

protected:

    // From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

    CProcessActive();

private:

    QProcess* process;
    RProcess** pproc;
    int serial;
    int deathPipe;
    bool errorValue;
};

// Active object to communicate synchronously with process manager thread
class CProcessManagerMediator : public CActive
{
public:
    static CProcessManagerMediator* construct();

    virtual ~CProcessManagerMediator();

    bool add(CProcessActive* processObserver);
    void remove(CProcessActive* processObserver);
    void terminate();

protected:

    enum Commands {
        ENoCommand,
        EAdd,
        ERemove,
        ETerminate
    };

    // From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

    CProcessManagerMediator();

    bool notify(CProcessActive* processObserver, Commands command);

private:
    CProcessActive* currentObserver;
    Commands currentCommand;

    RThread processManagerThread;
};

// Process manager manages child process death listeners
class QProcessManager
{
public:
    QProcessManager();
    ~QProcessManager();

    void startThread();

    TInt run(void* param);
    bool add(QProcess *process);
    void remove(QProcess *process);

    inline void setMediator(CProcessManagerMediator* newMediator) {mediator = newMediator;};

private:
    inline void lock() {managerMutex.Wait();};
    inline void unlock() {managerMutex.Signal();};

    QMap<int, CProcessActive *> children;
    CProcessManagerMediator* mediator;
    RMutex managerMutex;
    bool threadStarted;
    RThread managerThread;
};

static bool qt_rprocess_running(RProcess* proc)
{
    if(proc && proc->Handle()) {
        TExitType et = proc->ExitType();
        if (et == EExitPending) {
            return true;
        }
    }

    return false;
}

static void qt_create_symbian_commandline(const QStringList &arguments, QString& commandLine)
{
    for (int i=0; i<arguments.size(); ++i) {
        QString tmp = arguments.at(i);
        // in the case of \" already being in the string the \ must also be escaped
        tmp.replace( QLatin1String("\\\""), QLatin1String("\\\\\"") );
        // escape a single " because the arguments will be parsed
        tmp.replace( QLatin1String("\""), QLatin1String("\\\"") );
        if (tmp.isEmpty() || tmp.contains(QLatin1Char(' ')) || tmp.contains(QLatin1Char('\t'))) {
            // The argument must not end with a \ since this would be interpreted
            // as escaping the quote -- rather put the \ behind the quote: e.g.
            // rather use "foo"\ than "foo\"
            QString endQuote(QLatin1String("\""));
            int i = tmp.length();
            while (i>0 && tmp.at(i-1) == QLatin1Char('\\')) {
                --i;
                endQuote += QLatin1String("\\");
            }
            commandLine += QLatin1String(" \"") + tmp.left(i) + endQuote;
        } else {
            commandLine += QLatin1Char(' ') + tmp;
        }
    }
}

static TInt qt_create_symbian_process(RProcess **proc, const QString &programName, const QStringList &arguments)
{
    RProcess* newProc = NULL;
    newProc = new RProcess();

    if (!newProc) {
        return KErrNoMemory;
    }

    QString commandLine;
    qt_create_symbian_commandline(arguments, commandLine);

    TPtrC program_ptr(reinterpret_cast<const TText*>(programName.constData()));
    TPtrC cmdline_ptr(reinterpret_cast<const TText*>(commandLine.constData()));

    TInt err = newProc->Create(program_ptr, cmdline_ptr);

    if (err == KErrNotFound){
        // Strip path from program name and try again (i.e. try from default location "\sys\bin")
        int index = programName.lastIndexOf(QChar('\\'));
        int index2 = programName.lastIndexOf(QChar('/'));
        index = qMax(index, index2);

        if(index != -1 && programName.length() >= index){
            QString strippedName;
            strippedName = programName.mid(index+1);
            QPROCESS_DEBUG_PRINT("qt_create_symbian_process() Process '%s' not found, try stripped version '%s'", qPrintable(programName), qPrintable(strippedName));

            TPtrC stripped_ptr(reinterpret_cast<const TText*>(strippedName.constData()));
            err = newProc->Create(stripped_ptr, cmdline_ptr);

            if (err != KErrNone) {
                QPROCESS_DEBUG_PRINT("qt_create_symbian_process() Unable to create process '%s': %d", qPrintable(strippedName), err);
            }
        }
    }

    if (err == KErrNone) {
        *proc = newProc;
    } else {
        delete newProc;
    }

    return err;
}

static qint64 qt_native_read(int fd, char *data, qint64 maxlen)
{
    qint64 ret = 0;
    do {
        ret = ::read(fd, data, maxlen);
    } while (ret == -1 && errno == EINTR);

    QPROCESS_DEBUG_PRINT("qt_native_read(): fd: %d, result: %d, errno = %d", fd, (int)ret, errno);

    return ret;
}

static qint64 qt_native_write(int fd, const char *data, qint64 len)
{
    qint64 ret = 0;
    do {
        ret = ::write(fd, data, len);
    } while (ret == -1 && errno == EINTR);

    QPROCESS_DEBUG_PRINT("qt_native_write(): fd: %d, result: %d, errno = %d", fd, (int)ret, errno);

    return ret;
}

static void qt_native_close(int fd)
{
    int ret;
    do {
        ret = ::close(fd);
    } while (ret == -1 && errno == EINTR);
}

static void qt_create_pipe(int *pipe)
{
    if (pipe[0] != -1)
        qt_native_close(pipe[0]);
    if (pipe[1] != -1)
        qt_native_close(pipe[1]);
    if (::pipe(pipe) != 0) {
        qWarning("QProcessPrivate::createPipe: Cannot create pipe %p: %s",
                 pipe, qPrintable(qt_error_string(errno)));
    } else {
        QPROCESS_DEBUG_PRINT("qt_create_pipe(): Created pipe %d - %d", pipe[0], pipe[1]);
    }
}

// Called from ProcessManagerThread
CProcessActive* CProcessActive::construct(QProcess* process,
                                          RProcess** proc,
                                          int serial,
                                          int deathPipe)
{
    QPROCESS_ASSERT((process || proc || *proc),
        EProcessActiveNullParameter,
        "CProcessActive::construct(): process (0x%x), proc (0x%x) or *proc == NULL, not creating an instance", process, proc)

    CProcessActive*  newInstance = new CProcessActive();

    if (!newInstance) {
        QPROCESS_DEBUG_PRINT("CProcessActive::construct(): Failed to create new instance");
    } else {
        newInstance->process = process;
        newInstance->pproc = proc;
        newInstance->serial = serial;
        newInstance->deathPipe = deathPipe;
        newInstance->errorValue = false;
    }

    return newInstance;
}

// Called from ProcessManagerThread
CProcessActive::CProcessActive()
    : CActive(CActive::EPriorityStandard)
{
    // Nothing to do
}

// Called from ProcessManagerThread
CProcessActive::~CProcessActive()
{
    process = NULL;
    pproc = NULL;
}

// Called from ProcessManagerThread
void CProcessActive::start()
{
    if (qt_rprocess_running(*pproc)) {
        CActiveScheduler::Add(this);
        (*pproc)->Logon(iStatus);
        SetActive();
        QPROCESS_DEBUG_PRINT("CProcessActive::start(): Started monitoring for process exit.");
    } else {
        QPROCESS_DEBUG_PRINT("CProcessActive::start(): Process doesn't exist or is already dead");
        // Assume process has already died
        qt_native_write(deathPipe, "", 1);
        errorValue = true;
    }
}

// Called from ProcessManagerThread
void CProcessActive::stop()
{
    QPROCESS_DEBUG_PRINT("CProcessActive::stop()");

    // Remove this from scheduler (also cancels the request)
    Deque();
}

bool CProcessActive::error()
{
    return errorValue;
}

// Called from ProcessManagerThread
void CProcessActive::RunL()
{
    // If this method gets executed, the monitored process has died

    // Notify main thread
    qt_native_write(deathPipe, "", 1);
    QPROCESS_DEBUG_PRINT("CProcessActive::RunL() sending death notice to %d", deathPipe);
}

// Called from ProcessManagerThread
TInt CProcessActive::RunError(TInt aError)
{
    Q_UNUSED(aError);
    // Handle RunL leave (should never happen)
    QPROCESS_ASSERT(0, EProcessActiveRunError, "CProcessActive::RunError(): Should never get here!")
    return 0;
}

// Called from ProcessManagerThread
void CProcessActive::DoCancel()
{
    QPROCESS_DEBUG_PRINT("CProcessActive::DoCancel()");

    if (qt_rprocess_running(*pproc)) {
        (*pproc)->LogonCancel(iStatus);
        QPROCESS_DEBUG_PRINT("CProcessActive::DoCancel(): Stopped monitoring for process exit.");
    } else {
        QPROCESS_DEBUG_PRINT("CProcessActive::DoCancel(): Process doesn't exist");
    }
}


// Called from ProcessManagerThread
CProcessManagerMediator* CProcessManagerMediator::construct()
{
    CProcessManagerMediator*  newInstance = new CProcessManagerMediator;
    TInt err(KErrNone);

    newInstance->currentCommand = ENoCommand;
    newInstance->currentObserver = NULL;

    if (newInstance) {
        err = newInstance->processManagerThread.Open(newInstance->processManagerThread.Id());
        QPROCESS_ASSERT((err == KErrNone),
            EProcessManagerMediatorThreadOpenFailed,
            "CProcessManagerMediator::construct(): Failed to open processManagerThread (err:%d)", err)
    } else {
        QPROCESS_ASSERT(0,
            EProcessManagerMediatorCreationFailed,
            "CProcessManagerMediator::construct(): Failed to open construct mediator")
    }

    // Activate mediator
    CActiveScheduler::Add(newInstance);
    newInstance->iStatus = KRequestPending;
    newInstance->SetActive();
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::construct(): new instance successfully created and activated");

    return newInstance;
}

// Called from ProcessManagerThread
CProcessManagerMediator::CProcessManagerMediator()
    : CActive(CActive::EPriorityStandard)
{
    // Nothing to do
}

// Called from ProcessManagerThread
CProcessManagerMediator::~CProcessManagerMediator()
{
    processManagerThread.Close();
	currentCommand = ENoCommand;
    currentObserver = NULL;
}

// Called from main thread
bool CProcessManagerMediator::add(CProcessActive* processObserver)
{
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::add()");
    return notify(processObserver, EAdd);
}

// Called from main thread
void CProcessManagerMediator::remove(CProcessActive* processObserver)
{
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::remove()");
    notify(processObserver, ERemove);
}

// Called from main thread
void CProcessManagerMediator::terminate()
{
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::terminate()");
    notify(NULL, ETerminate);
}

// Called from main thread
bool CProcessManagerMediator::notify(CProcessActive* processObserver, Commands command)
{
    bool success(true);

    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::Notify(): Command: %d, processObserver: 0x%x", command, processObserver);

    QPROCESS_ASSERT((command == ETerminate || processObserver),
        EProcessManagerMediatorNullObserver,
        "CProcessManagerMediator::Notify(): NULL processObserver not allowed for command: %d", command)

    QPROCESS_ASSERT(IsActive(),
        EProcessManagerMediatorInactive,
        "CProcessManagerMediator::Notify(): Mediator is not active!")

    QPROCESS_ASSERT(iStatus==KRequestPending,
        EProcessManagerMediatorNotPending,
        "CProcessManagerMediator::Notify(): Mediator request not pending!")

    currentObserver = processObserver;
    currentCommand = command;

    // Sync with process manager thread
    TRequestStatus pmStatus;
    processManagerThread.Rendezvous(pmStatus);

    // Complete request -> RunL will run in the process manager thread
    TRequestStatus* status = &iStatus;
    processManagerThread.RequestComplete(status, command);

    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::Notify(): Waiting process manager to complete...");
    User::WaitForRequest(pmStatus);
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::Notify(): Wait over");

    if (currentObserver) {
        success = !(currentObserver->error());
        QPROCESS_DEBUG_PRINT("CProcessManagerMediator::Notify(): success = %d", success);
    }

    currentObserver = NULL;
    currentCommand = ENoCommand;

    return success;
}

// Called from ProcessManagerThread
void CProcessManagerMediator::RunL()
{
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::RunL(): currentCommand: %d, iStatus: %d", currentCommand, iStatus.Int());
    switch (currentCommand) {
        case EAdd:
            currentObserver->start();
            break;
        case ERemove:
            currentObserver->stop();
            break;
        case ETerminate:
            Deque();
            CActiveScheduler::Stop();
            return;
        default:
            QPROCESS_ASSERT(0,
                EProcessManagerMediatorInvalidCmd,
                "CProcessManagerMediator::RunL(): Invalid command!")
            break;
    }

    iStatus = KRequestPending;
    SetActive();

    // Notify main thread that we are done
    RThread::Rendezvous(KErrNone);
}

// Called from ProcessManagerThread
TInt CProcessManagerMediator::RunError(TInt aError)
{
    Q_UNUSED(aError);
    // Handle RunL leave (should never happen)
    QPROCESS_ASSERT(0,
        EProcessManagerMediatorRunError,
        "CProcessManagerMediator::RunError(): Should never get here!")
    return 0;
}

// Called from ProcessManagerThread
void CProcessManagerMediator::DoCancel()
{
    QPROCESS_DEBUG_PRINT("CProcessManagerMediator::DoCancel()");
    TRequestStatus* status = &iStatus;
    processManagerThread.RequestComplete(status, KErrCancel);
}

Q_GLOBAL_STATIC(QProcessManager, processManager)

TInt processManagerThreadFunction(TAny* param)
{
    QPROCESS_ASSERT(param,
        EProcessManagerNullParam,
        "processManagerThreadFunction(): NULL param")

    QProcessManager* manager = reinterpret_cast<QProcessManager*>(param);

    CActiveScheduler* scheduler = new CQtActiveScheduler();

    QPROCESS_ASSERT(scheduler,
        EProcessManagerSchedulerCreationFail,
        "processManagerThreadFunction(): Scheduler creation failed")

    CActiveScheduler::Install(scheduler);

    //Creating mediator also adds it to scheduler and activates it. Failure will panic.
    manager->setMediator(CProcessManagerMediator::construct());
    RThread::Rendezvous(KErrNone);

    CActiveScheduler::Start();

    CActiveScheduler::Install(NULL);
    delete scheduler;

    return KErrNone;
}

QProcessManager::QProcessManager()
    : mediator(NULL), threadStarted(false)
{
    TInt err = managerMutex.CreateLocal();

    QPROCESS_ASSERT(err == KErrNone,
        EProcessManagerMutexCreationFail,
        "QProcessManager::QProcessManager(): Failed to create new managerMutex (err: %d)", err)
}

QProcessManager::~QProcessManager()
{
    QPROCESS_DEBUG_PRINT("QProcessManager::~QProcessManager()");
    // Cancel death listening for all child processes
    if (mediator) {
        QMap<int, CProcessActive *>::Iterator it = children.begin();
        while (it != children.end()) {
            // Remove all monitors
            CProcessActive *active = it.value();
            mediator->remove(active);

            QPROCESS_DEBUG_PRINT("QProcessManager::~QProcessManager() removed listening for a process");
            ++it;
        }

        // Terminate process manager thread.
        mediator->terminate();
        delete mediator;
    }

    qDeleteAll(children.values());
    children.clear();
	managerThread.Close();
    managerMutex.Close();
}

void QProcessManager::startThread()
{
    lock();

    if (!threadStarted) {
        TInt err = managerThread.Create(KQProcessManagerThreadName,
                                        processManagerThreadFunction,
                                        0x5000,
                                        (RAllocator*)NULL,
                                        (TAny*)this,
                                        EOwnerProcess);

        QPROCESS_ASSERT(err == KErrNone,
            EProcessManagerThreadCreationFail,
            "QProcessManager::startThread(): Failed to create new managerThread (err:%d)", err)

        threadStarted = true;

        // Manager thread must start running before we continue, so sync with rendezvous
        TRequestStatus status;
        managerThread.Rendezvous(status);
        managerThread.Resume();
        User::WaitForRequest(status);
    }

    unlock();
}

static QBasicAtomicInt idCounter = Q_BASIC_ATOMIC_INITIALIZER(1);

bool QProcessManager::add(QProcess *process)
{
    QPROCESS_ASSERT(process,
        EProcessManagerNullParam,
        "QProcessManager::add(): Failed to add CProcessActive to ProcessManager - NULL process")

    lock();

    int serial = idCounter.fetchAndAddRelaxed(1);
    process->d_func()->serial = serial;

    QPROCESS_DEBUG_PRINT("QProcessManager::add(): serial: %d, deathPipe: %d - %d, symbianProcess: 0x%x", serial, process->d_func()->deathPipe[0], process->d_func()->deathPipe[1], process->d_func()->symbianProcess);

    CProcessActive* newActive =
        CProcessActive::construct(process,
                                  &(process->d_func()->symbianProcess),
                                  serial,
                                  process->d_func()->deathPipe[1]);

    if (newActive){
        if (mediator->add(newActive)) {
            children.insert(serial, newActive);
            unlock();
            return true;
        } else {
            QPROCESS_DEBUG_PRINT("QProcessManager::add(): Failed to add CProcessActive to ProcessManager");
            delete newActive;
        }
    }

    unlock();

    return false;
}

void QProcessManager::remove(QProcess *process)
{
    QPROCESS_ASSERT(process,
        EProcessManagerNullParam,
        "QProcessManager::remove(): Failed to remove CProcessActive from ProcessManager - NULL process")

    lock();

    int serial = process->d_func()->serial;
    CProcessActive *active = children.value(serial);
    if (!active) {
        unlock();
        return;
    }

    mediator->remove(active);

    children.remove(serial);
    delete active;

    unlock();
}

void QProcessPrivate::destroyPipe(int *pipe)
{
    if (pipe[1] != -1) {
        qt_native_close(pipe[1]);
        pipe[1] = -1;
    }
    if (pipe[0] != -1) {
        qt_native_close(pipe[0]);
        pipe[0] = -1;
    }
}

bool QProcessPrivate::createChannel(Channel &channel)
{
    Q_UNUSED(channel);
    // No channels used
    return false;
}

void QProcessPrivate::startProcess()
{
    Q_Q(QProcess);

    QPROCESS_DEBUG_PRINT("QProcessPrivate::startProcess()");

    // Start the process (platform dependent)
    q->setProcessState(QProcess::Starting);

    processManager()->startThread();

    qt_create_pipe(deathPipe);
    if (threadData->eventDispatcher) {
        deathNotifier = new QSocketNotifier(deathPipe[0],
                                            QSocketNotifier::Read, q);
        QObject::connect(deathNotifier, SIGNAL(activated(int)),
                         q, SLOT(_q_processDied()));
    }

    TInt err = qt_create_symbian_process(&symbianProcess, program, arguments);

    if (err == KErrNone) {
        pid = symbianProcess->Id();

        ::fcntl(deathPipe[0], F_SETFL, ::fcntl(deathPipe[0], F_GETFL) | O_NONBLOCK);

        if (!processManager()->add(q)) {
            qWarning("QProcessPrivate::startProcess(): Failed to start monitoring for process death.");
            err = KErrNoMemory;
        }
    }

    if (err != KErrNone) {
        // Cleanup, report error and return
        QPROCESS_DEBUG_PRINT("QProcessPrivate::startProcess() Process open failed, err: %d, '%s'", err, qPrintable(program));
        q->setProcessState(QProcess::NotRunning);
        processError = QProcess::FailedToStart;
        q->setErrorString(QLatin1String(QT_TRANSLATE_NOOP(QProcess, "Resource error (qt_create_symbian_process failure)")));
        emit q->error(processError);
        cleanup();
        return;
    }

    processLaunched = true;

    symbianProcess->Resume();

    QPROCESS_DEBUG_PRINT("QProcessPrivate::startProcess(): this: 0x%x, pid: %d", this, (TUint)pid);

    // Notify child start
    _q_startupNotification();

}

bool QProcessPrivate::processStarted()
{
    QPROCESS_DEBUG_PRINT("QProcessPrivate::processStarted() == %s", processLaunched ? "true" : "false");

    // Since we cannot get information whether process has actually been launched
    // or not in Symbian, we need to fake it. Assume process is started if launch was
    // successful.

    return processLaunched;
}

qint64 QProcessPrivate::bytesAvailableFromStdout() const
{
    // In Symbian, zero bytes are always available
    return 0;
}

qint64 QProcessPrivate::bytesAvailableFromStderr() const
{
    // In Symbian, zero bytes are always available
    return 0;
}

qint64 QProcessPrivate::readFromStdout(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    // In Symbian, zero bytes are always read
    return 0;
}

qint64 QProcessPrivate::readFromStderr(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    // In Symbian, zero bytes are always read
    return 0;
}

qint64 QProcessPrivate::writeToStdin(const char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    // In Symbian, zero bytes are always written
    return 0;
}

void QProcessPrivate::terminateProcess()
{
    // Not allowed by platform security - will panic kern-exec 46 if process has been started.
    // Works if process is not yet started.
    if (qt_rprocess_running(symbianProcess)) {
        symbianProcess->Terminate(0);
    } else {
        QPROCESS_DEBUG_PRINT("QProcessPrivate::terminateProcess(), Process not running");
    }
}

void QProcessPrivate::killProcess()
{
    // Not allowed by platform security - will panic kern-exec 46 if process has been started.
    // Works if process is not yet started.
    if (qt_rprocess_running(symbianProcess)) {
        symbianProcess->Kill(0);
    } else {
        QPROCESS_DEBUG_PRINT("QProcessPrivate::killProcess(), Process not running");
    }
}

bool QProcessPrivate::waitForStarted(int msecs)
{
    Q_UNUSED(msecs);
    // Since we can get no actual feedback from process beyond its death,
    // assume that started has already been emitted if process has been launched
    return processLaunched;
}

bool QProcessPrivate::waitForReadyRead(int msecs)
{
    // Functionality not supported in Symbian
    Q_UNUSED(msecs);
    return false;
}

bool QProcessPrivate::waitForBytesWritten(int msecs)
{
    // Functionality not supported in Symbian
    Q_UNUSED(msecs);
    return false;
}

bool QProcessPrivate::waitForFinished(int msecs)
{
    Q_Q(QProcess);
    QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForFinished(%d)", msecs);

    TRequestStatus timerStatus = 0;
    TRequestStatus logonStatus = 0;
    bool timeoutOccurred = false;

    // Logon to process to observe its death
    if (qt_rprocess_running(symbianProcess)) {
        symbianProcess->Logon(logonStatus);

        // Create timer
        RTimer timer;
        timer.CreateLocal();
        TTimeIntervalMicroSeconds32 interval(msecs*1000);
        timer.After(timerStatus, interval);

        QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForFinished() - Waiting...");
        User::WaitForRequest(logonStatus, timerStatus);
        QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForFinished() - Wait completed");

        if (timerStatus == KErrNone) {
            timeoutOccurred = true;
        }

        timer.Cancel();
        timer.Close();

        symbianProcess->LogonCancel(logonStatus);

        // Eat cancel request completion so that it won't mess up main thread scheduling later
        User::WaitForRequest(logonStatus, timerStatus);
    } else {
        QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForFinished(), qt_rprocess_running returned false");
    }

	if (timeoutOccurred) {
	    processError = QProcess::Timedout;
	    q->setErrorString(QLatin1String(QT_TRANSLATE_NOOP(QProcess, "Process operation timed out")));
	    return false;
	}

    _q_processDied();

    return true;
}

bool QProcessPrivate::waitForWrite(int msecs)
{
    // Functionality not supported in Symbian
    Q_UNUSED(msecs);
    return false;
}

// Deceptively named function. Exit code is actually got in waitForDeadChild().
void QProcessPrivate::findExitCode()
{
    Q_Q(QProcess);
    processManager()->remove(q);
}

bool QProcessPrivate::waitForDeadChild()
{
    Q_Q(QProcess);

    // read a byte from the death pipe
    char c;
    qt_native_read(deathPipe[0], &c, 1);

    if (symbianProcess && symbianProcess->Handle()) {
        TExitType et = symbianProcess->ExitType();
        QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForDeadChild() symbianProcess->ExitType: %d", et);
        if (et != EExitPending) {
            processManager()->remove(q);
            exitCode = symbianProcess->ExitReason();
            crashed = (et == EExitPanic);
#if defined QPROCESS_DEBUG
            TExitCategoryName catName = symbianProcess->ExitCategory();
            qDebug() << "QProcessPrivate::waitForDeadChild() dead with exitCode"
                     << exitCode << ", crashed:" << crashed
                     << ", category:" << QString::fromUtf16(catName.Ptr());
#endif
        } else {
            QPROCESS_DEBUG_PRINT("QProcessPrivate::waitForDeadChild() not dead!");
        }
    }

    return true;
}

void QProcessPrivate::_q_notified()
{
    // Nothing to do in Symbian
}

/*! \internal
 */
bool QProcessPrivate::startDetached(const QString &program, const QStringList &arguments, const QString &workingDirectory, qint64 *pid)
{
    QPROCESS_DEBUG_PRINT("QProcessPrivate::startDetached()");
    Q_UNUSED(workingDirectory);

    RProcess* newProc = NULL;

    TInt err = qt_create_symbian_process(&newProc, program, arguments);

    if (err == KErrNone) {
        if (pid) {
            *pid = (qint64)newProc->Id();
        }

        newProc->Resume();
        newProc->Close();
        return true;
    }

    return false;
}


void QProcessPrivate::initializeProcessManager()
{
    (void) processManager();
}

QT_END_NAMESPACE

#endif // QT_NO_PROCESS
