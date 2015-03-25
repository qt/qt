/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <remconinterfaceselector.h>
#include <remconcoreapitarget.h>
#include <coemain.h>
#include <private/qkeymapper_p.h>
#include "qs60keycapture_p.h"

QT_BEGIN_NAMESPACE

/*
 * Helper class for sending key handling responses to RemCon.
*/
class CResponseHandler : public CActive
{
public:
    static CResponseHandler *NewL(CRemConCoreApiTarget &remConCoreApiTarget);
    virtual ~CResponseHandler();
    void CompleteAnyKey(TRemConCoreApiOperationId operationId);

private:
    CResponseHandler(CRemConCoreApiTarget &remConCoreApiTarget);

    void RunL();
    void DoCancel();

private:
    RArray<TRemConCoreApiOperationId> iResponseArray;
    CRemConCoreApiTarget &iRemConCoreApiTarget;
};

CResponseHandler::CResponseHandler(CRemConCoreApiTarget &remConCoreApiTarget)
    : CActive(CActive::EPriorityStandard),
      iRemConCoreApiTarget(remConCoreApiTarget)
{
    CActiveScheduler::Add(this);
}

CResponseHandler *CResponseHandler::NewL(CRemConCoreApiTarget &remConCoreApiTarget)
{
    CResponseHandler *self =
        new (ELeave) CResponseHandler(remConCoreApiTarget);
    return self;
}

CResponseHandler::~CResponseHandler()
{
    Cancel();
    iResponseArray.Close();
}

void CResponseHandler::CompleteAnyKey(TRemConCoreApiOperationId operationId)
{
    if (!IsActive()) {
        TInt error = KErrNone;
        iRemConCoreApiTarget.SendResponse(iStatus, operationId, error);
        SetActive();
    } else {
        // already active. Append to array and complete later.
        iResponseArray.Append(operationId);
    }
}

void CResponseHandler::DoCancel()
{
    iRemConCoreApiTarget.Cancel();
}

void CResponseHandler::RunL()
{
    // if any existing -> Send response
    if (iResponseArray.Count()) {
        CompleteAnyKey(iResponseArray[0]);
        // Remove already completed key
        iResponseArray.Remove(0);
        iResponseArray.Compress();
    }
}


/*
 * QS60KeyCapture provides media key handling using services from RemCon.
 */
QS60KeyCapture::QS60KeyCapture(CCoeEnv *env, QObject *parent):
    QObject(parent), coeEnv(env), selector(0), target(0), handler(0)
{
    initRemCon();

    TTimeIntervalMicroSeconds32 initialTime;
    TTimeIntervalMicroSeconds32 time;
    coeEnv->WsSession().GetKeyboardRepeatRate(initialTime, time);
    initialRepeatTime = (initialTime.Int() / 1000); // msecs
    repeatTime = (time.Int() / 1000); // msecs

    int clickTimeout = initialRepeatTime + repeatTime;

    volumeUpClickTimer.setSingleShot(true);
    volumeDownClickTimer.setSingleShot(true);
    repeatTimer.setSingleShot(true);

    volumeUpClickTimer.setInterval(clickTimeout);
    volumeDownClickTimer.setInterval(clickTimeout);
    repeatTimer.setInterval(initialRepeatTime);

    connect(&volumeUpClickTimer, SIGNAL(timeout()), this, SLOT(volumeUpClickTimerExpired()));
    connect(&volumeDownClickTimer, SIGNAL(timeout()), this, SLOT(volumeDownClickTimerExpired()));
    connect(&repeatTimer, SIGNAL(timeout()), this, SLOT(repeatTimerExpired()));
}

/*
 * Initializes RemCon connection for receiving key events
 */
void QS60KeyCapture::initRemCon()
{
    try {
        QT_TRAP_THROWING(
                selector = CRemConInterfaceSelector::NewL();
                target = CRemConCoreApiTarget::NewL(*selector, *this);
                handler = CResponseHandler::NewL(*target);
                selector->OpenTargetL());
    } catch (const std::exception &e) {
        delete handler;
        delete selector;
        selector = 0;
        target = 0;
        handler = 0;
    }
}

QS60KeyCapture::~QS60KeyCapture()
{
    delete handler;
    delete selector;
}

void QS60KeyCapture::MrccatoCommand(TRemConCoreApiOperationId operationId,
                                    TRemConCoreApiButtonAction buttonAction)
{
    if (!target)
        return;

    switch (operationId) {
    // Volume up/down keys auto repeat if user hold the key long enough
    case ERemConCoreApiVolumeUp:
    case ERemConCoreApiVolumeDown:
        {
        // Side key events are sent using following scheme:
        //  - ButtonClick is sent first
        //  - if nothing happens before repeat timer expires, no further events are generated
        //  - if user holds the button longer, ButtonPress is sent and
        //    when button is finally released ButtonRelease is sent.

        QTimer &timer = (operationId == ERemConCoreApiVolumeUp) ? volumeUpClickTimer : volumeDownClickTimer;

        switch (buttonAction) {
        case ERemConCoreApiButtonPress:
            if (timer.isActive()) {
                timer.stop();
                // force repeat event
                repeatKeyEvent = mapToKeyEvent(operationId);
                repeatTimerExpired();
            } else {
                sendKey(operationId, QEvent::KeyPress);
                repeatTimer.start(initialRepeatTime);
            }
            break;
        case ERemConCoreApiButtonRelease:
            timer.stop();
            sendKey(operationId, QEvent::KeyRelease);
            repeatTimer.stop();
            break;
        case ERemConCoreApiButtonClick:
            if (timer.isActive()) {
                timer.stop();
                sendKey(operationId, QEvent::KeyRelease);
            }
            sendKey(operationId, QEvent::KeyPress);
            timer.start();
            repeatTimer.stop();
            break;
        default:
            break;
        }
        }
        break;
    default:
        switch (buttonAction) {
        case ERemConCoreApiButtonPress:
            sendKey(operationId, QEvent::KeyPress);
            repeatTimer.start(initialRepeatTime);
            break;
        case ERemConCoreApiButtonRelease:
            sendKey(operationId, QEvent::KeyRelease);
            repeatTimer.stop();
            break;
        case ERemConCoreApiButtonClick:
            sendKey(operationId, QEvent::KeyPress);
            sendKey(operationId, QEvent::KeyRelease);
            repeatTimer.stop();
            break;
        default:
            break;
        }
        break;
    }

    if (handler)
        handler->CompleteAnyKey(operationId);
}

/*
 * Sends volume up KeyRelease event
 */
void QS60KeyCapture::volumeUpClickTimerExpired()
{
    sendKey(ERemConCoreApiVolumeUp, QEvent::KeyRelease);
}

/*
 * Sends volume down KeyRelease event
 */
void QS60KeyCapture::volumeDownClickTimerExpired()
{
    sendKey(ERemConCoreApiVolumeDown, QEvent::KeyRelease);
}

/*
 * Repeats last key event
 */
void QS60KeyCapture::repeatTimerExpired()
{
    // set auto repeat flag on
    repeatKeyEvent.iRepeats = 1;
    coeEnv->SimulateKeyEventL(repeatKeyEvent, EEventKey);
    repeatTimer.start(repeatTime);
}

/*
 * Maps RemCon operation id to key event (includes key code and scan codes)
 */
TKeyEvent QS60KeyCapture::mapToKeyEvent(TRemConCoreApiOperationId operationId)
{
    TKeyEvent keyEvent;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats = 0;
    keyEvent.iCode = qt_keymapper_private()->mapS60RemConIdToS60Key(operationId);
    keyEvent.iScanCode = qt_keymapper_private()->mapS60RemConIdToS60ScanCodes(operationId);
    return keyEvent;
}

/*
 * Delivers key events to the application.
 * RemCon operations are converted to simulated key events
 */
void QS60KeyCapture::sendKey(TRemConCoreApiOperationId operationId, QEvent::Type type)
{
    TKeyEvent keyEvent = mapToKeyEvent(operationId);

    if (type == QEvent::KeyPress) {
        coeEnv->SimulateKeyEventL(keyEvent, EEventKeyDown);
        coeEnv->SimulateKeyEventL(keyEvent, EEventKey);
        // save the key press event for repeats
        repeatKeyEvent = keyEvent;
    } else {
        coeEnv->SimulateKeyEventL(keyEvent, EEventKeyUp);
    }
}

QT_END_NAMESPACE
