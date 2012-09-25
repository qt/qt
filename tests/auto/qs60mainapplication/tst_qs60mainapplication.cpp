/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtGui/QS60MainApplication>
#include <QtGui/QS60MainDocument>
#include <QtGui/QS60MainAppUi>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QS60MainApplication : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void customQS60MainApplication();
    void testMultimediaKeys_data();
    void testMultimediaKeys();
};

void tst_QS60MainApplication::initTestCase()
{
}

void tst_QS60MainApplication::cleanupTestCase()
{
}

void tst_QS60MainApplication::init()
{
}

void tst_QS60MainApplication::cleanup()
{
}

#ifdef Q_WS_S60
bool appUiConstructed = false;

class CustomMainAppUi : public QS60MainAppUi
{
public:
    CustomMainAppUi()
    {
        appUiConstructed = true;
    }
};

class CustomMainDocument : public QS60MainDocument
{
public:
    CustomMainDocument(CEikApplication &eikApp)
        : QS60MainDocument(eikApp)
    {
    }
    CEikAppUi *CreateAppUiL()
    {
        return new (ELeave) CustomMainAppUi;
    }
};

class CustomMainApplication : public QS60MainApplication
{
protected:
    CApaDocument *CreateDocumentL()
    {
        return new (ELeave) CustomMainDocument(*this);
    }
};

CApaApplication *factory()
{
    return new (ELeave) CustomMainApplication;
}

#include <remconcoreapicontrollerobserver.h>
#include <remconcoreapicontroller.h>
#include <remconinterfaceselector.h>
#include <QTimer>
#include <QSignalSpy>

class KeyGenerator : public QObject,
                     public MRemConCoreApiControllerObserver
{
    Q_OBJECT
public:
    KeyGenerator(QObject *parent = 0);
    ~KeyGenerator();
    void MrccacoResponse(TRemConCoreApiOperationId operationId, TInt error);

    void simulateKey(int qtKey);

private:
    void init();
    void cleanup();

    CRemConInterfaceSelector *interfaceSelector;
    CRemConCoreApiController *coreController;
};

KeyGenerator::KeyGenerator(QObject *parent) : QObject(parent)
{
    init();
}

KeyGenerator::~KeyGenerator()
{
    cleanup();
}

void KeyGenerator::MrccacoResponse(TRemConCoreApiOperationId operationId, TInt error)
{
    Q_UNUSED(operationId);
    Q_UNUSED(error);
}

/*
 * Generates keyPress and keyRelease events for given key
 */
void KeyGenerator::simulateKey(int qtKey)
{
    if (!coreController)
        return;

    TRemConCoreApiButtonAction action = ERemConCoreApiButtonClick;
    TUint numRemotes = 0;
    TRequestStatus status;
    bool wait = true;

    switch (qtKey) {
    // media keys
    case Qt::Key_VolumeUp:
        coreController->VolumeUp(status, numRemotes, action);
        break;
    case Qt::Key_VolumeDown:
        coreController->VolumeDown(status, numRemotes, action);
        break;
    case Qt::Key_MediaStop:
        coreController->Stop(status, numRemotes, action);
        break;
    case Qt::Key_MediaTogglePlayPause:
        coreController->PausePlayFunction(status, numRemotes, action);
        break;
    case Qt::Key_MediaNext:
        coreController->Forward(status, numRemotes, action);
        break;
    case Qt::Key_MediaPrevious:
        coreController->Backward(status, numRemotes, action);
        break;
    case Qt::Key_AudioForward:
        coreController->FastForward(status, numRemotes, action);
        break;
    case Qt::Key_AudioRewind:
        coreController->Rewind(status, numRemotes, action);
        break;
    // accessory keys
    case Qt::Key_Select:
        coreController->Select(status, numRemotes, action);
        break;
    case Qt::Key_Enter:
        coreController->Enter(status, numRemotes, action);
        break;
    case Qt::Key_PageUp:
        coreController->PageUp(status, numRemotes, action);
        break;
    case Qt::Key_PageDown:
        coreController->PageDown(status, numRemotes, action);
        break;
    case Qt::Key_Left:
        coreController->Left(status, numRemotes, action);
        break;
    case Qt::Key_Right:
        coreController->Right(status, numRemotes, action);
        break;
    case Qt::Key_Up:
        coreController->Up(status, numRemotes, action);
        break;
    case Qt::Key_Down:
        coreController->Down(status, numRemotes, action);
        break;
    case Qt::Key_Help:
        coreController->Help(status, numRemotes, action);
        break;
    case Qt::Key_F1:
        coreController->F1(status, numRemotes, action);
        break;
    case Qt::Key_F2:
        coreController->F2(status, numRemotes, action);
        break;
    case Qt::Key_F3:
        coreController->F3(status, numRemotes, action);
        break;
    case Qt::Key_F4:
        coreController->F4(status, numRemotes, action);
        break;
    case Qt::Key_F5:
        coreController->F5(status, numRemotes, action);
        break;
    default:
        wait = false;
        break;
    }

    if (wait)
        User::WaitForRequest(status);
}

void KeyGenerator::init()
{
    try {
        QT_TRAP_THROWING(interfaceSelector = CRemConInterfaceSelector::NewL());
        QT_TRAP_THROWING(coreController = CRemConCoreApiController::NewL(*interfaceSelector, *this));
        QT_TRAP_THROWING(interfaceSelector->OpenControllerL());
    } catch (const std::exception &e) {
        cleanup();
    }
}

void KeyGenerator::cleanup()
{
    delete interfaceSelector;
    interfaceSelector = 0;
    coreController = 0;
}

const int keyEventTimeout = 2000; // 2secs

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    TestWidget(QWidget *parent = 0);
    ~TestWidget();

signals:
    void keyPress(int key);
    void keyRelease(int key);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QTimer exitTimer;
};

TestWidget::TestWidget(QWidget *parent) : QWidget(parent)
{
    // quit if no events are received
    exitTimer.setSingleShot(true);
    exitTimer.start(keyEventTimeout);
    connect(&exitTimer, SIGNAL(timeout()), qApp, SLOT(quit()));
}

TestWidget::~TestWidget()
{
}

void TestWidget::keyPressEvent(QKeyEvent *event)
{
    emit keyPress(event->key());
}

void TestWidget::keyReleaseEvent(QKeyEvent *event)
{
    emit keyRelease(event->key());
    qApp->quit(); // test is done so quit immediately
}

#endif // Q_WS_S60

void tst_QS60MainApplication::customQS60MainApplication()
{
#ifndef Q_WS_S60
    QSKIP("This is an S60-only test", SkipAll);
#else
    int argc = 1;
    char *argv = "tst_qs60mainapplication";
    QApplication app(factory, argc, &argv);
    QVERIFY(appUiConstructed);
#endif
}

void tst_QS60MainApplication::testMultimediaKeys_data()
{
    QTest::addColumn<int>("key");

    QTest::newRow("Key_VolumeUp") << (int)Qt::Key_VolumeUp;
    QTest::newRow("Key_VolumeDown") << (int)Qt::Key_VolumeDown;
    QTest::newRow("Key_MediaStop") << (int)Qt::Key_MediaStop;
    QTest::newRow("Key_MediaTogglePlayPause") << (int)Qt::Key_MediaTogglePlayPause;
    QTest::newRow("Key_MediaNext") << (int)Qt::Key_MediaNext;
    QTest::newRow("Key_MediaPrevious") << (int)Qt::Key_MediaPrevious;
    QTest::newRow("Key_AudioForward") << (int)Qt::Key_AudioForward;
    QTest::newRow("Key_AudioRewind") << (int)Qt::Key_AudioRewind;

    QTest::newRow("Key_Select") << (int)Qt::Key_Select;
    QTest::newRow("Key_Enter") << (int)Qt::Key_Enter;
    QTest::newRow("Key_PageUp") << (int)Qt::Key_PageUp;
    QTest::newRow("Key_PageDown") << (int)Qt::Key_PageDown;
    QTest::newRow("Key_Left") << (int)Qt::Key_Left;
    QTest::newRow("Key_Right") << (int)Qt::Key_Right;
    QTest::newRow("Key_Up") << (int)Qt::Key_Up;
    QTest::newRow("Key_Down") << (int)Qt::Key_Down;
    QTest::newRow("Key_Help") << (int)Qt::Key_Help;
    QTest::newRow("Key_F1") << (int)Qt::Key_F1;
    QTest::newRow("Key_F2") << (int)Qt::Key_F2;
    QTest::newRow("Key_F3") << (int)Qt::Key_F3;
    QTest::newRow("Key_F4") << (int)Qt::Key_F4;
    QTest::newRow("Key_F5") << (int)Qt::Key_F5;
}

void tst_QS60MainApplication::testMultimediaKeys()
{
#ifndef Q_WS_S60
    QSKIP("This is an S60-only test", SkipAll);
#elif __WINS__
    QSKIP("S60 emulator not supported", SkipAll);
#else
    QApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);
    int argc = 1;
    char *argv = "tst_qs60mainapplication";
    QApplication app(argc, &argv);

    QFETCH(int, key);
    KeyGenerator keyGen;
    keyGen.simulateKey(key);

    TestWidget widget;
    QSignalSpy keyPressSpy(&widget, SIGNAL(keyPress(int)));
    QSignalSpy keyReleaseSpy(&widget, SIGNAL(keyRelease(int)));

    widget.show();
    app.exec();

    QCOMPARE(keyPressSpy.count(), 1);
    QList<QVariant> arguments = keyPressSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == key);

    QCOMPARE(keyReleaseSpy.count(), 1);
    arguments = keyReleaseSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == key);
#endif
}

QTEST_APPLESS_MAIN(tst_QS60MainApplication)
#include "tst_qs60mainapplication.moc"
