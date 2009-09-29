/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qapplication_p.h"
#include "qsessionmanager.h"
#include "qevent.h"
#include "qeventdispatcher_s60_p.h"
#include "qwidget.h"
#include "qdesktopwidget.h"
#include "private/qbackingstore_p.h"
#include "qt_s60_p.h"
#include "private/qevent_p.h"
#include "qstring.h"
#include "qdebug.h"
#include "qimage.h"
#include "private/qkeymapper_p.h"
#include "private/qfont_p.h"
#ifndef QT_NO_STYLE_S60
#include "private/qs60style_p.h"
#endif
#include "private/qwindowsurface_s60_p.h"
#include "qpaintengine.h"
#include "private/qmenubar_p.h"
#include "private/qsoftkeymanager_p.h"

#include "apgwgnam.h" // For CApaWindowGroupName
#include <MdaAudioTonePlayer.h>     // For CMdaAudioToneUtility

#if defined(Q_WS_S60)
# if !defined(QT_NO_IM)
#  include "qinputcontext.h"
#  include <private/qcoefepinputcontext_p.h>
# endif
# include <private/qs60mainapplication_p.h>
#endif

#include "private/qstylesheetstyle_p.h"

#include <hal.h>
#include <hal_data.h>

QT_BEGIN_NAMESPACE

#if defined(QT_DEBUG)
static bool        appNoGrab        = false;        // Grabbing enabled
#endif
static bool        app_do_modal        = false;        // modal mode
Q_GLOBAL_STATIC(QS60Data, qt_s60Data);

extern bool qt_sendSpontaneousEvent(QObject*,QEvent*);
extern QWidgetList *qt_modal_stack;              // stack of modal widgets
extern QDesktopWidget *qt_desktopWidget; // qapplication.cpp

QWidget *qt_button_down = 0;                     // widget got last button-down

QS60Data* qGlobalS60Data()
{
    return qt_s60Data();
}

bool qt_nograb()                                // application no-grab option
{
#if defined(QT_DEBUG)
    return appNoGrab;
#else
    return false;
#endif
}

// Modified from http://www3.symbian.com/faq.nsf/0/0F1464EE96E737E780256D5E00503DD1?OpenDocument
class QS60Beep : public CBase, public MMdaAudioToneObserver
{
public:
    static QS60Beep* NewL(TInt aFrequency,  TTimeIntervalMicroSeconds iDuration);
    void Play();
    ~QS60Beep();
private:
    void ConstructL(TInt aFrequency,  TTimeIntervalMicroSeconds iDuration);
    void MatoPrepareComplete(TInt aError);
    void MatoPlayComplete(TInt aError);
private:
    typedef enum
        {
        EBeepNotPrepared,
        EBeepPrepared,
        EBeepPlaying
        } TBeepState;
private:
    CMdaAudioToneUtility* iToneUtil;
    TBeepState iState;
    TInt iFrequency;
    TTimeIntervalMicroSeconds iDuration;
};

QS60Beep::~QS60Beep()
{
    delete iToneUtil;
}

QS60Beep* QS60Beep::NewL(TInt aFrequency, TTimeIntervalMicroSeconds aDuration)
{
    QS60Beep* self=new (ELeave) QS60Beep();
    CleanupStack::PushL(self);
    self->ConstructL(aFrequency, aDuration);
    CleanupStack::Pop();
    return self;
};

void QS60Beep::ConstructL(TInt aFrequency, TTimeIntervalMicroSeconds aDuration)
{
    iToneUtil=CMdaAudioToneUtility::NewL(*this);
    iState=EBeepNotPrepared;
    iFrequency=aFrequency;
    iDuration=aDuration;
    iToneUtil->PrepareToPlayTone(iFrequency,iDuration);
}

void QS60Beep::Play()
{
    if (iState != EBeepNotPrepared) {
        if (iState == EBeepPlaying) {
            iToneUtil->CancelPlay();
            iState = EBeepPrepared;
        }
    }

    iToneUtil->Play();
    iState = EBeepPlaying;
}

void QS60Beep::MatoPrepareComplete(TInt aError)
{
    if (aError == KErrNone) {
        iState = EBeepPrepared;
    }
}

void QS60Beep::MatoPlayComplete(TInt aError)
{
    Q_UNUSED(aError);
    iState=EBeepPrepared;
}


QHash<TInt, TUint> QApplicationPrivate::scanCodeCache;

static Qt::KeyboardModifiers mapToQtModifiers(TUint s60Modifiers)
{
    Qt::KeyboardModifiers result = Qt::NoModifier;

    if (s60Modifiers & EModifierKeypad)
        result |= Qt::KeypadModifier;
    if (s60Modifiers & EModifierShift || s60Modifiers & EModifierLeftShift
            || s60Modifiers & EModifierRightShift)
        result |= Qt::ShiftModifier;
    if (s60Modifiers & EModifierCtrl || s60Modifiers & EModifierLeftCtrl
            || s60Modifiers & EModifierRightCtrl)
        result |= Qt::ControlModifier;
    if (s60Modifiers & EModifierAlt || s60Modifiers & EModifierLeftAlt
            || s60Modifiers & EModifierRightAlt)
        result |= Qt::AltModifier;

    return result;
}

static void mapS60MouseEventTypeToQt(QEvent::Type *type, Qt::MouseButton *button, const TPointerEvent *pEvent)
{
    switch (pEvent->iType) {
    case TPointerEvent::EButton1Down:
        *type = QEvent::MouseButtonPress;
        *button = Qt::LeftButton;
        break;
    case TPointerEvent::EButton1Up:
        *type = QEvent::MouseButtonRelease;
        *button = Qt::LeftButton;
        break;
    case TPointerEvent::EButton2Down:
        *type = QEvent::MouseButtonPress;
        *button = Qt::MidButton;
        break;
    case TPointerEvent::EButton2Up:
        *type = QEvent::MouseButtonRelease;
        *button = Qt::MidButton;
        break;
    case TPointerEvent::EButton3Down:
        *type = QEvent::MouseButtonPress;
        *button = Qt::RightButton;
        break;
    case TPointerEvent::EButton3Up:
        *type = QEvent::MouseButtonRelease;
        *button = Qt::RightButton;
        break;
    case TPointerEvent::EDrag:
        *type = QEvent::MouseMove;
        *button = Qt::NoButton;
        break;
    case TPointerEvent::EMove:
        // Qt makes no distinction between move and drag
        *type = QEvent::MouseMove;
        *button = Qt::NoButton;
        break;
    default:
        *type = QEvent::None;
        *button = Qt::NoButton;
        break;
    }
    if (pEvent->iModifiers & EModifierDoubleClick){
        *type = QEvent::MouseButtonDblClick;
    }

    if (*type == QEvent::MouseButtonPress || *type == QEvent::MouseButtonDblClick)
        QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons | (*button);
    else if (*type == QEvent::MouseButtonRelease)
        QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons &(~(*button));

    QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons & Qt::MouseButtonMask;
}

//### Can be replaced with CAknLongTapDetector if animation is required.
//NOTE: if CAknLongTapDetector is used make sure it gets variated out of 3.1 and 3.2,.
//also MLongTapObserver needs to be changed to MAknLongTapDetectorCallBack if CAknLongTapDetector is used.
class QLongTapTimer : public CTimer
{
public:
    static QLongTapTimer* NewL(QAbstractLongTapObserver *observer);
    QLongTapTimer(QAbstractLongTapObserver *observer);
    void ConstructL();
public:
    void PointerEventL(const TPointerEvent &event);
    void RunL();
protected:
private:
    QAbstractLongTapObserver *m_observer;
    TPointerEvent m_event;
    QPoint m_pressedCoordinates;
    int m_dragDistance;
};

QLongTapTimer* QLongTapTimer::NewL(QAbstractLongTapObserver *observer)
{
    QLongTapTimer* self = new QLongTapTimer(observer);
    self->ConstructL();
    return self;
}
void QLongTapTimer::ConstructL()
{
    CTimer::ConstructL();
}

QLongTapTimer::QLongTapTimer(QAbstractLongTapObserver *observer):CTimer(CActive::EPriorityHigh)
{
    m_observer = observer;
    m_dragDistance = qApp->startDragDistance();
    CActiveScheduler::Add(this);
}

void QLongTapTimer::PointerEventL(const TPointerEvent& event)
{
    if ( event.iType == TPointerEvent::EDrag || event.iType == TPointerEvent::EButtonRepeat)
    {
        QPoint diff(QPoint(event.iPosition.iX,event.iPosition.iY) - m_pressedCoordinates);
        if (diff.manhattanLength() < m_dragDistance)
            return;
    }
    Cancel();
    m_event = event;
    if (event.iType == TPointerEvent::EButton1Down)
    {
        m_pressedCoordinates = QPoint(event.iPosition.iX,event.iPosition.iY);
        // must be same as KLongTapDelay in aknlongtapdetector.h
        After(800000);
    }
}
void QLongTapTimer::RunL()
{
    if (m_observer)
        m_observer->HandleLongTapEventL(m_event.iPosition, m_event.iParentPosition);
}

QSymbianControl::QSymbianControl(QWidget *w)
    : CCoeControl(), qwidget(w), m_ignoreFocusChanged(false)
{
}

void QSymbianControl::ConstructL(bool topLevel, bool desktop)
{
    if (!desktop)
    {
        if (topLevel) {
            CreateWindowL(S60->windowGroup());
        }

        SetFocusing(true);
        m_longTapDetector = QLongTapTimer::NewL(this);
    }
}

QSymbianControl::~QSymbianControl()
{
    if (S60->curWin == this)
        S60->curWin = 0;
    S60->appUi()->RemoveFromStack(this);
    delete m_longTapDetector;
}

void QSymbianControl::setWidget(QWidget *w)
{
    qwidget = w;
}
void QSymbianControl::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation )
{
    QWidget *alienWidget;
    QPoint widgetPos = QPoint(aPenEventLocation.iX, aPenEventLocation.iY);
    QPoint globalPos = QPoint(aPenEventScreenLocation.iX,aPenEventScreenLocation.iY);
    alienWidget = qwidget->childAt(widgetPos);
    if (!alienWidget)
        alienWidget = qwidget;
    QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons &(~Qt::LeftButton);
    QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons | Qt::RightButton;
    QMouseEvent mEvent(QEvent::MouseButtonPress, alienWidget->mapFrom(qwidget, widgetPos), globalPos,
        Qt::RightButton, QApplicationPrivate::mouse_buttons, Qt::NoModifier);

    bool res = sendMouseEvent(alienWidget, &mEvent);

#if !defined(QT_NO_CONTEXTMENU)
    QContextMenuEvent contextMenuEvent(QContextMenuEvent::Mouse, widgetPos, globalPos, mEvent.modifiers());
    qt_sendSpontaneousEvent(alienWidget, &contextMenuEvent);
#endif

    m_previousEventLongTap = true;
}

#ifdef QT_SYMBIAN_SUPPORTS_ADVANCED_POINTER
void QSymbianControl::translateAdvancedPointerEvent(const TAdvancedPointerEvent *event)
{
    QApplicationPrivate *d = QApplicationPrivate::instance();

    QRect screenGeometry = qApp->desktop()->screenGeometry(qwidget);

    while (d->appAllTouchPoints.count() <= event->PointerNumber())
        d->appAllTouchPoints.append(QTouchEvent::TouchPoint(d->appAllTouchPoints.count()));

    Qt::TouchPointStates allStates = 0;
    for (int i = 0; i < d->appAllTouchPoints.count(); ++i) {
        QTouchEvent::TouchPoint &touchPoint = d->appAllTouchPoints[i];

        if (touchPoint.id() == event->PointerNumber()) {
            Qt::TouchPointStates state;
            switch (event->iType) {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EEnterHighPressure:
                state = Qt::TouchPointPressed;
                break;
            case TPointerEvent::EButton1Up:
            case TPointerEvent::EExitCloseProximity:
                state = Qt::TouchPointReleased;
                break;
            case TPointerEvent::EDrag:
                state = Qt::TouchPointMoved;
                break;
            default:
                // how likely is this to happen?
                state = Qt::TouchPointStationary;
                break;
            }
            if (event->PointerNumber() == 0)
                state |= Qt::TouchPointPrimary;
            touchPoint.setState(state);

            QPointF screenPos = QPointF(event->iPosition.iX, event->iPosition.iY);
            touchPoint.setScreenPos(screenPos);
            touchPoint.setNormalizedPos(QPointF(screenPos.x() / screenGeometry.width(),
                                                screenPos.y() / screenGeometry.height()));

            touchPoint.setPressure(event->Pressure() / qreal(d->maxTouchPressure));
        } else if (touchPoint.state() != Qt::TouchPointReleased) {
            // all other active touch points should be marked as stationary
            touchPoint.setState(Qt::TouchPointStationary);
        }

        allStates |= touchPoint.state();
    }

    if ((allStates & Qt::TouchPointStateMask) == Qt::TouchPointReleased) {
        // all touch points released
        d->appAllTouchPoints.clear();
    }

    QApplicationPrivate::translateRawTouchEvent(qwidget,
                                                QTouchEvent::TouchScreen,
                                                d->appAllTouchPoints);
}
#endif

void QSymbianControl::HandlePointerEventL(const TPointerEvent& pEvent)
{
#ifdef QT_SYMBIAN_SUPPORTS_ADVANCED_POINTER
    if (pEvent.IsAdvancedPointerEvent()) {
        const TAdvancedPointerEvent *advancedPointerEvent = pEvent.AdvancedPointerEvent();
        translateAdvancedPointerEvent(advancedPointerEvent);
        if (advancedPointerEvent->PointerNumber() != 0) {
            // only send mouse events for the first touch point
            return;
        }
    }
#endif

    m_longTapDetector->PointerEventL(pEvent);
    QT_TRYCATCH_LEAVING(HandlePointerEvent(pEvent));
}

void QSymbianControl::HandlePointerEvent(const TPointerEvent& pEvent)
{
    //### refactor me, getting too complex
    QMouseEvent::Type type;
    Qt::MouseButton button;
    mapS60MouseEventTypeToQt(&type, &button, &pEvent);

    if (m_previousEventLongTap)
        if (type == QEvent::MouseButtonRelease){
            button = Qt::RightButton;
            QApplicationPrivate::mouse_buttons = QApplicationPrivate::mouse_buttons & ~Qt::RightButton;
            m_previousEventLongTap = false;
        }
    if (type == QMouseEvent::None)
        return;

    // store events for later sending/saving
    QWidget *alienWidget;
    typedef QPair<QWidget*,QMouseEvent> Event;
    QList<Event > events;

    QPoint widgetPos = QPoint(pEvent.iPosition.iX, pEvent.iPosition.iY);
    TPoint controlScreenPos = PositionRelativeToScreen();
    QPoint globalPos = QPoint(controlScreenPos.iX, controlScreenPos.iY) + widgetPos;

    if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick || type == QEvent::MouseMove)
    {
        // get the widget where the event happened
        alienWidget = qwidget->childAt(widgetPos);
        if (!alienWidget)
            alienWidget = qwidget;
        S60->mousePressTarget = alienWidget;
    }

    alienWidget = S60->mousePressTarget;

    if (alienWidget != S60->lastPointerEventTarget)
        if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick || type == QEvent::MouseMove)
        {
            //moved to another widget, create enter and leave events
            if (S60->lastPointerEventTarget)
            {
                QMouseEvent mEventLeave(QEvent::Leave, S60->lastPointerEventTarget->mapFromGlobal(S60->lastCursorPos), S60->lastCursorPos,
                    button, QApplicationPrivate::mouse_buttons, mapToQtModifiers(pEvent.iModifiers));
                events.append(Event(S60->lastPointerEventTarget,mEventLeave));
            }
            if (alienWidget) {
                QMouseEvent mEventEnter(QEvent::Enter, alienWidget->mapFromGlobal(globalPos),
                    globalPos, button, QApplicationPrivate::mouse_buttons, mapToQtModifiers(
                        pEvent.iModifiers));

                events.append(Event(alienWidget, mEventEnter));
#ifndef QT_NO_CURSOR
                S60->curWin = alienWidget->effectiveWinId();
                if (!QApplication::overrideCursor()) {
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
                    if (S60->brokenPointerCursors)
                        qt_symbian_set_pointer_sprite(alienWidget->cursor());
                    else
#endif
                        qt_symbian_setWindowCursor(alienWidget->cursor(), S60->curWin);
                }
#endif
            }
        }
    S60->lastCursorPos = globalPos;
#if !defined(QT_NO_CURSOR) && !defined(Q_SYMBIAN_FIXED_POINTER_CURSORS)
    if (S60->brokenPointerCursors)
        qt_symbian_move_cursor_sprite();
#endif
    S60->lastPointerEventPos = widgetPos;
    S60->lastPointerEventTarget = alienWidget;
    if (alienWidget)
    {
        QMouseEvent mEvent(type, alienWidget->mapFromGlobal(globalPos), globalPos,
            button, QApplicationPrivate::mouse_buttons, mapToQtModifiers(pEvent.iModifiers));
        events.append(Event(alienWidget,mEvent));
        QEventDispatcherS60 *dispatcher;
        // It is theoretically possible for someone to install a different event dispatcher.
        if ((dispatcher = qobject_cast<QEventDispatcherS60 *>(alienWidget->d_func()->threadData->eventDispatcher)) != 0) {
            if (dispatcher->excludeUserInputEvents()) {
                for (int i=0;i < events.count();++i)
                {
                    Event next = events[i];
                    dispatcher->saveInputEvent(this, next.first, new QMouseEvent(next.second));
                }
                return;
            }
        }
    }
    for (int i=0;i < events.count();++i)
    {
        Event next = events[i];
        sendMouseEvent(next.first, &(next.second));
    }
}

bool QSymbianControl::sendMouseEvent(QWidget *widget, QMouseEvent *mEvent)
{
    return qt_sendSpontaneousEvent(widget, mEvent);
}

TKeyResponse QSymbianControl::OfferKeyEventL(const TKeyEvent& keyEvent, TEventCode type)
{
    TKeyResponse r = EKeyWasNotConsumed;
    QT_TRYCATCH_LEAVING(r = OfferKeyEvent(keyEvent, type));
    return r;
}

TKeyResponse QSymbianControl::OfferKeyEvent(const TKeyEvent& keyEvent, TEventCode type)
{
    switch (type) {
    //case EEventKeyDown: // <-- Intentionally left out. See below.
    case EEventKeyUp:
    case EEventKey:
    {
        // S60 has a confusing way of delivering key events. There are three types of
        // events: EKeyEvent, EKeyEventDown and EKeyEventUp. When a key is pressed, the
        // two first events are generated. When releasing the key, the last one is
        // generated.
        // Because S60 does not generate keysyms for EKeyEventDown and EKeyEventUp events,
        // we need to do some special tricks to map it to the Qt way. First, we completely
        // discard EKeyEventDown events, since they are redundant. Second, since
        // EKeyEventUp does not give us a keysym, we need to cache the keysyms from
        // the EKeyEvent events. This is what resolveS60ScanCode does.


        // ### hackish way to send Qt application to background when pressing right softkey
        /*
        if( keyEvent.iScanCode == EStdKeyDevice1 ) {
            S60->window_group->SetOrdinalPosition(-1);
            qApp->setActiveWindow(0);
            return EKeyWasNotConsumed;
        }
        */

        TUint s60Keysym = QApplicationPrivate::resolveS60ScanCode(keyEvent.iScanCode,
                keyEvent.iCode);
        int keyCode;
        if (s60Keysym >= 0x20 && s60Keysym < ENonCharacterKeyBase) {
            // Normal characters keys.
            keyCode = s60Keysym;
        } else {
            // Special S60 keys.
            keyCode = qt_keymapper_private()->mapS60KeyToQt(s60Keysym);
        }

#ifndef QT_NO_CURSOR
        if (S60->mouseInteractionEnabled && S60->virtualMouseRequired) {
            //translate keys to pointer
            if (keyCode >= Qt::Key_Left && keyCode <= Qt::Key_Down || keyCode == Qt::Key_Select) {
                /*Explanation about virtualMouseAccel:
                 Tapping an arrow key allows precise pixel positioning
                 Holding an arrow key down, acceleration is applied to allow cursor
                 to be quickly moved to another part of the screen by key repeats.
                 */
                if (S60->virtualMouseLastKey == keyCode) {
                    S60->virtualMouseAccel *= 2;
                    if (S60->virtualMouseAccel > S60->virtualMouseMaxAccel)
                        S60->virtualMouseAccel = S60->virtualMouseMaxAccel;
                }
                else
                    S60->virtualMouseAccel = 1;
                S60->virtualMouseLastKey = keyCode;

                QPoint pos = QCursor::pos();
                TPointerEvent fakeEvent;
                TInt x = pos.x();
                TInt y = pos.y();
                if (type == EEventKeyUp) {
                    if (keyCode == Qt::Key_Select)
                        fakeEvent.iType = TPointerEvent::EButton1Up;
                    S60->virtualMouseAccel = 1;
                    S60->virtualMouseLastKey = 0;
                }
                else if (type == EEventKey) {
                    switch (keyCode) {
                    case Qt::Key_Left:
                        x -= S60->virtualMouseAccel;
                        fakeEvent.iType = TPointerEvent::EMove;
                        break;
                    case Qt::Key_Right:
                        x += S60->virtualMouseAccel;
                        fakeEvent.iType = TPointerEvent::EMove;
                        break;
                    case Qt::Key_Up:
                        y -= S60->virtualMouseAccel;
                        fakeEvent.iType = TPointerEvent::EMove;
                        break;
                    case Qt::Key_Down:
                        y += S60->virtualMouseAccel;
                        fakeEvent.iType = TPointerEvent::EMove;
                        break;
                    case Qt::Key_Select:
                        fakeEvent.iType = TPointerEvent::EButton1Down;
                        break;
                    }
                }
                //clip to screen size (window server allows a sprite hotspot to be outside the screen)
                if (x < 0)
                    x = 0;
                else if (x >= S60->screenWidthInPixels)
                    x = S60->screenWidthInPixels - 1;
                if (y < 0)
                    y = 0;
                else if (y >= S60->screenHeightInPixels)
                    y = S60->screenHeightInPixels - 1;
                TPoint epos(x, y);
                TPoint cpos = epos - PositionRelativeToScreen();
                fakeEvent.iModifiers = keyEvent.iModifiers;
                fakeEvent.iPosition = cpos;
                fakeEvent.iParentPosition = epos;
                HandlePointerEvent(fakeEvent);
                return EKeyWasConsumed;
            }
            else {
                S60->virtualMouseLastKey = keyCode;
                S60->virtualMouseAccel = 1;
            }
        }
#endif

        Qt::KeyboardModifiers mods = mapToQtModifiers(keyEvent.iModifiers);
        QKeyEventEx qKeyEvent(type == EEventKeyUp ? QEvent::KeyRelease : QEvent::KeyPress, keyCode,
                mods, qt_keymapper_private()->translateKeyEvent(keyCode, mods),
                false, 1, keyEvent.iScanCode, s60Keysym, mods);
//        WId wid = reinterpret_cast<RWindowGroup *>(keyEvent.Handle())->Child();
//        if (!wid)
//             Could happen if window isn't shown yet.
//            return EKeyWasNotConsumed;
        QWidget *widget;
        widget = QWidget::keyboardGrabber();
        if (!widget) {
            if (QApplicationPrivate::popupWidgets != 0) {
                widget = QApplication::activePopupWidget()->focusWidget();
                if (!widget) {
                    widget = QApplication::activePopupWidget();
                }
            } else {
                widget = QApplicationPrivate::focus_widget;
                if (!widget) {
                    widget = qwidget;
                }
            }
        }

        QEventDispatcherS60 *dispatcher;
        // It is theoretically possible for someone to install a different event dispatcher.
        if ((dispatcher = qobject_cast<QEventDispatcherS60 *>(widget->d_func()->threadData->eventDispatcher)) != 0) {
            if (dispatcher->excludeUserInputEvents()) {
                dispatcher->saveInputEvent(this, widget, new QKeyEventEx(qKeyEvent));
                return EKeyWasConsumed;
            }
        }
        return sendKeyEvent(widget, &qKeyEvent);
    }
    }
    return EKeyWasNotConsumed;
}

void QSymbianControl::sendInputEvent(QWidget *widget, QInputEvent *inputEvent)
{
    switch (inputEvent->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        sendKeyEvent(widget, static_cast<QKeyEvent *>(inputEvent));
        break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        sendMouseEvent(widget, static_cast<QMouseEvent *>(inputEvent));
        break;
    default:
        // Shouldn't get here.
        Q_ASSERT_X(0 == 1, "QSymbianControl::sendInputEvent()", "inputEvent->type() is unknown");
        break;
    }
}

TKeyResponse QSymbianControl::sendKeyEvent(QWidget *widget, QKeyEvent *keyEvent)
{
#if !defined(QT_NO_IM) && defined(Q_WS_S60)
    if (widget && widget->isEnabled() && widget->testAttribute(Qt::WA_InputMethodEnabled)) {
        QInputContext *qic = widget->inputContext();
        if (qic && qic->filterEvent(keyEvent))
            return EKeyWasConsumed;
    }
#endif // !defined(QT_NO_IM) && defined(Q_WS_S60)

    if (widget && qt_sendSpontaneousEvent(widget, keyEvent))
        if (keyEvent->isAccepted())
            return EKeyWasConsumed;

    return EKeyWasNotConsumed;
}

#if !defined(QT_NO_IM) && defined(Q_WS_S60)
TCoeInputCapabilities QSymbianControl::InputCapabilities() const
{
    QWidget *w = 0;

    if (qwidget->hasFocus())
        w = qwidget;
    else
        w = qwidget->focusWidget();

    QCoeFepInputContext *ic;
    if (w && w->isEnabled() && w->testAttribute(Qt::WA_InputMethodEnabled)
            && (ic = qobject_cast<QCoeFepInputContext *>(w->inputContext()))) {
        return ic->inputCapabilities();
    } else {
        return TCoeInputCapabilities(TCoeInputCapabilities::ENone, 0, 0);
    }
}
#endif

void QSymbianControl::Draw(const TRect& r) const
{
    QWindowSurface *surface = qwidget->windowSurface();
    if (!surface)
        return;

    QPaintEngine *engine = surface->paintDevice()->paintEngine();
    if (!engine)
        return;
    if (engine->type() == QPaintEngine::Raster) {
        QS60WindowSurface *s60Surface = static_cast<QS60WindowSurface *>(qwidget->windowSurface());
        CFbsBitmap *bitmap = s60Surface->symbianBitmap();
        CWindowGc &gc = SystemGc();
        if (qwidget->d_func()->isOpaque)
            gc.SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        gc.BitBlt(r.iTl, bitmap, r);
    } else {
        surface->flush(qwidget, QRegion(qt_TRect2QRect(r)), QPoint());
    }
}

void QSymbianControl::SizeChanged()
{
    CCoeControl::SizeChanged();

    QSize oldSize = qwidget->size();
    QSize newSize(Size().iWidth, Size().iHeight);

    if (oldSize != newSize) {
        QRect cr = qwidget->geometry();
        cr.setSize(newSize);
        qwidget->data->crect = cr;
        if (qwidget->isVisible()) {
            QTLWExtra *tlwExtra = qwidget->d_func()->maybeTopData();
            bool slowResize = qgetenv("QT_SLOW_TOPLEVEL_RESIZE").toInt();
            if (!slowResize && tlwExtra)
                tlwExtra->inTopLevelResize = true;
            QResizeEvent e(newSize, oldSize);
            qt_sendSpontaneousEvent(qwidget, &e);
            if (!qwidget->testAttribute(Qt::WA_StaticContents))
                qwidget->d_func()->syncBackingStore();
            if (!slowResize && tlwExtra)
                tlwExtra->inTopLevelResize = false;
        }
    }
}

void QSymbianControl::PositionChanged()
{
    CCoeControl::PositionChanged();

    QPoint oldPos = qwidget->geometry().topLeft();
    QPoint newPos(Position().iX, Position().iY);

    if (oldPos != newPos) {
        QRect cr = qwidget->geometry();
        cr.moveTopLeft(newPos);
        qwidget->data->crect = cr;
        QTLWExtra *top = qwidget->d_func()->maybeTopData();
        if (top)
            top->normalGeometry = cr;
        if (qwidget->isVisible()) {
            QMoveEvent e(newPos, oldPos);
            qt_sendSpontaneousEvent(qwidget, &e);
        } else {
            QMoveEvent * e = new QMoveEvent(newPos, oldPos);
            QApplication::postEvent(qwidget, e);
        }
    }
}

void QSymbianControl::FocusChanged(TDrawNow /* aDrawNow */)
{
    if (m_ignoreFocusChanged)
        return;

    // Popups never get focused, but still receive the FocusChanged when they are hidden.
    if (QApplicationPrivate::popupWidgets != 0
            || (qwidget->windowType() & Qt::Popup) == Qt::Popup)
        return;

    if (IsFocused()) {
        QApplication::setActiveWindow(qwidget);
#ifdef Q_WS_S60
        // If widget is fullscreen, hide status pane and button container
        // otherwise show them.
        CEikStatusPane* statusPane = S60->statusPane();
        CEikButtonGroupContainer* buttonGroup = S60->buttonGroupContainer();
        bool isFullscreen = qwidget->windowState() & Qt::WindowFullScreen;
        if (statusPane && (statusPane->IsVisible() == isFullscreen))
            statusPane->MakeVisible(!isFullscreen);
        if (buttonGroup && (buttonGroup->IsVisible() == isFullscreen))
            buttonGroup->MakeVisible(!isFullscreen);
#endif
    } else {
        QApplication::setActiveWindow(0);
    }
}

void QSymbianControl::HandleResourceChange(int resourceType)
{
    switch (resourceType) {
    case KInternalStatusPaneChange:
        qwidget->d_func()->setWindowIcon_sys(true);
        break;
    case KUidValueCoeFontChangeEvent:
        // font change event
        break;
#ifdef Q_WS_S60
    case KEikDynamicLayoutVariantSwitch:
    {
        if (qwidget->isFullScreen()) {
            SetExtentToWholeScreen();
        } else if (qwidget->isMaximized()) {
            TRect r = static_cast<CEikAppUi*>(S60->appUi())->ClientRect();
            SetExtent(r.iTl, r.Size());
        }
        break;
    }
#endif
    default:
        break;
    }

    CCoeControl::HandleResourceChange(resourceType);

}
void QSymbianControl::CancelLongTapTimer()
{
    m_longTapDetector->Cancel();
}

TTypeUid::Ptr QSymbianControl::MopSupplyObject(TTypeUid id)
{
    if (id.iUid == ETypeId)
        return id.MakePtr(this);

    return CCoeControl::MopSupplyObject(id);
}

/*!
    \typedef QApplication::QS60MainApplicationFactory

    This is a typedef for a pointer to a function with the following
    signature:

    \snippet doc/src/snippets/code/src_corelib_global_qglobal.cpp 47

    \sa QApplication::QApplication(QApplication::QS60MainApplicationFactory, int &, char **)
*/

/*!
    \since 4.6

    Creates an application using the application factory given in
    \a factory, and using \a argc command line arguments in \a argv.
    \a factory can be leaving, but the error will be converted to a
    standard exception.

    This function is only available on S60.
*/
QApplication::QApplication(QApplication::QS60MainApplicationFactory factory, int &argc, char **argv)
    : QCoreApplication(*new QApplicationPrivate(argc, argv, GuiClient))
{
    Q_D(QApplication);
    S60->s60ApplicationFactory = factory;
    d->construct();
}

QApplication::QApplication(QApplication::QS60MainApplicationFactory factory, int &argc, char **argv, int _internal)
    : QCoreApplication(*new QApplicationPrivate(argc, argv, GuiClient))
{
    Q_D(QApplication);
    S60->s60ApplicationFactory = factory;
    d->construct();
    QApplicationPrivate::app_compile_version = _internal;
}

void qt_init(QApplicationPrivate * /* priv */, int)
{
    if (!CCoeEnv::Static()) {
        // The S60 framework has not been initalized. We need to do it.
        TApaApplicationFactory factory(S60->s60ApplicationFactory ?
                S60->s60ApplicationFactory : newS60Application);
        CApaCommandLine* commandLine = 0;
        TInt err = CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
        // After this construction, CEikonEnv will be available from CEikonEnv::Static().
        // (much like our qApp).
        CEikonEnv* coe = new CEikonEnv;
        QT_TRAP_THROWING(coe->ConstructAppFromCommandLineL(factory,*commandLine));
        delete commandLine;

        S60->qtOwnsS60Environment = true;
    } else {
        S60->qtOwnsS60Environment = false;
    }

#ifdef QT_NO_DEBUG
    if (!qgetenv("QT_S60_AUTO_FLUSH_WSERV").isEmpty())
#endif
        S60->wsSession().SetAutoFlush(ETrue);

    S60->updateScreenSize();


    TDisplayMode mode = S60->screenDevice()->DisplayMode();
    S60->screenDepth = TDisplayModeUtils::NumDisplayModeBitsPerPixel(mode);

    RProcess me;
    TSecureId securId = me.SecureId();
    S60->uid = securId.operator TUid();

    // enable focus events - used to re-enable mouse after focus changed between mouse and non mouse app,
    // and for dimming behind modal windows
	S60->windowGroup().EnableFocusChangeEvents();

	//Check if mouse interaction is supported (either EMouse=1 in the HAL, or EMachineUID is one of the phones known to support this)
    const TInt KMachineUidSamsungI8510 = 0x2000C51E;
    TInt machineUID;
    TInt mouse;
    TInt touch;
    TInt err;
    err = HAL::Get(HALData::EMouse, mouse);
    if (err != KErrNone)
        mouse = 0;
    err = HAL::Get(HALData::EMachineUid, machineUID);
    if (err != KErrNone)
        machineUID = 0;
    err = HAL::Get(HALData::EPen, touch);
    if (err != KErrNone)
        touch = 0;
    if (mouse || machineUID == KMachineUidSamsungI8510) {
        S60->hasTouchscreen = false;
        S60->virtualMouseRequired = false;
    }
    else if (!touch) {
        S60->hasTouchscreen = false;
        S60->virtualMouseRequired = true;
    }
    else {
        S60->hasTouchscreen = true;
        S60->virtualMouseRequired = false;
    }

    if (touch) {
        QApplicationPrivate::navigationMode = Qt::NavigationModeNone;
    } else {
        QApplicationPrivate::navigationMode = Qt::NavigationModeKeypadDirectional;
    }

#ifndef QT_NO_CURSOR
    //Check if window server pointer cursors are supported or not
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
    //In generic binary, use the HAL and OS version
    //Any other known good phones should be added here.
    if (machineUID == KMachineUidSamsungI8510 || (QSysInfo::symbianVersion() != QSysInfo::SV_9_4
        && QSysInfo::symbianVersion() != QSysInfo::SV_9_3 && QSysInfo::symbianVersion()
        != QSysInfo::SV_9_2)) {
        S60->brokenPointerCursors = false;
        qt_symbian_setWindowGroupCursor(Qt::ArrowCursor, S60->windowGroup());
    }
    else
        S60->brokenPointerCursors = true;
#endif

    if (S60->mouseInteractionEnabled) {
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
        if (S60->brokenPointerCursors) {
            qt_symbian_set_pointer_sprite(Qt::ArrowCursor);
            qt_symbian_show_pointer_sprite();
        }
        else
#endif
            S60->wsSession().SetPointerCursorMode(EPointerCursorNormal);
    }
#endif

/*
 ### Commented out for now as parameter handling not needed in SOS(yet). Code below will break testlib with -o flag
    int argc = priv->argc;
    char **argv = priv->argv;

    // Get command line params
    int j = argc ? 1 : 0;
    for (int i=1; i<argc; i++) {
        if (argv[i] && *argv[i] != '-') {
            argv[j++] = argv[i];
            continue;
        }

#if defined(QT_DEBUG)
        if (qstrcmp(argv[i], "-nograb") == 0)
            appNoGrab = !appNoGrab;
        else
#endif // QT_DEBUG
            ;
    }
*/
}

/*****************************************************************************
  qt_cleanup() - cleans up when the application is finished
 *****************************************************************************/
void qt_cleanup()
{
    QFontCache::cleanup(); // Has to happen now, since QFontEngineS60 has FBS handles
// S60 structure and window server session are freed in eventdispatcher destructor as they are needed there

    // It's important that this happens here, before the event dispatcher gets
    // deleted, because the input context needs the event loop one last time before
    // it dies.
    delete QApplicationPrivate::inputContext;
    QApplicationPrivate::inputContext = 0;

    //Change mouse pointer back
    S60->wsSession().SetPointerCursorMode(EPointerCursorNone);

    if (S60->qtOwnsS60Environment) {
        CEikonEnv* coe = CEikonEnv::Static();
        coe->PrepareToExit();
        // The CEikonEnv itself is destroyed in here.
        coe->DestroyEnvironment();
    }
}

void QApplicationPrivate::initializeWidgetPaletteHash()
{
    // TODO: Implement QApplicationPrivate::initializeWidgetPaletteHash()
    // Possibly a task fot the S60Style guys
}

void QApplicationPrivate::createEventDispatcher()
{
    Q_Q(QApplication);
    eventDispatcher = new QEventDispatcherS60(q);
}

QString QApplicationPrivate::appName() const
{
    return QCoreApplicationPrivate::appName();
}

bool QApplicationPrivate::modalState()
{
    return app_do_modal;
}

void QApplicationPrivate::enterModal_sys(QWidget *widget)
{
    if (widget) {
        widget->effectiveWinId()->DrawableWindow()->FadeBehind(ETrue);
        // Modal partial screen dialogs (like queries) capture pointer events.
        // ### FixMe: Add specialized behaviour for fullscreen modal dialogs
        widget->effectiveWinId()->SetGloballyCapturing(ETrue);
        widget->effectiveWinId()->SetPointerCapture(ETrue);
    }
    if (!qt_modal_stack)
        qt_modal_stack = new QWidgetList;
    qt_modal_stack->insert(0, widget);
    app_do_modal = true;
}

void QApplicationPrivate::leaveModal_sys(QWidget *widget)
{
    if (widget) {
        widget->effectiveWinId()->DrawableWindow()->FadeBehind(EFalse);
        // ### FixMe: Add specialized behaviour for fullscreen modal dialogs
        widget->effectiveWinId()->SetGloballyCapturing(EFalse);
        widget->effectiveWinId()->SetPointerCapture(EFalse);
    }
    if (qt_modal_stack && qt_modal_stack->removeAll(widget)) {
        if (qt_modal_stack->isEmpty()) {
            delete qt_modal_stack;
            qt_modal_stack = 0;
        }
    }
    app_do_modal = qt_modal_stack != 0;
}

void QApplicationPrivate::openPopup(QWidget *popup)
{
    if (!QApplicationPrivate::popupWidgets)
        QApplicationPrivate::popupWidgets = new QWidgetList;
    QApplicationPrivate::popupWidgets->append(popup);


    // Cancel focus widget pointer capture and long tap timer
    if (QApplication::focusWidget()) {
        static_cast<QSymbianControl*>(QApplication::focusWidget()->effectiveWinId())->CancelLongTapTimer();
        QApplication::focusWidget()->effectiveWinId()->SetPointerCapture(false);
        }

    if (!qt_nograb()) {
        // Cancel pointer capture and long tap timer for earlier popup
        int popupCount = QApplicationPrivate::popupWidgets->count();
        if (popupCount > 1) {
            QWidget* prevPopup = QApplicationPrivate::popupWidgets->at(popupCount-2);
            static_cast<QSymbianControl*>(prevPopup->effectiveWinId())->CancelLongTapTimer();
            prevPopup->effectiveWinId()->SetPointerCapture(false);
        }

        // Enable pointer capture for this (topmost) popup
        Q_ASSERT(popup->testAttribute(Qt::WA_WState_Created));
        WId id = popup->effectiveWinId();
        id->SetPointerCapture(true);
    }

    // popups are not focus-handled by the window system (the first
    // popup grabbed the keyboard), so we have to do that manually: A
    // new popup gets the focus
    QWidget *fw = popup->focusWidget();
    if (fw) {
        fw->setFocus(Qt::PopupFocusReason);
    } else if (QApplicationPrivate::popupWidgets->count() == 1) { // this was the first popup
        fw = QApplication::focusWidget();
        if (fw) {
            QFocusEvent e(QEvent::FocusOut, Qt::PopupFocusReason);
            q_func()->sendEvent(fw, &e);
        }
    }
}

void QApplicationPrivate::closePopup(QWidget *popup)
{
    if (!QApplicationPrivate::popupWidgets)
        return;
    QApplicationPrivate::popupWidgets->removeAll(popup);

    // Cancel pointer capture and long tap for this popup
    WId id = popup->effectiveWinId();
    id->SetPointerCapture(false);
    static_cast<QSymbianControl*>(id)->CancelLongTapTimer();

    if (QApplicationPrivate::popupWidgets->isEmpty()) { // this was the last popup
        delete QApplicationPrivate::popupWidgets;
        QApplicationPrivate::popupWidgets = 0;
        if (!qt_nograb()) {                        // grabbing not disabled
            Q_ASSERT(popup->testAttribute(Qt::WA_WState_Created));
            if (QWidgetPrivate::mouseGrabber != 0)
                QWidgetPrivate::mouseGrabber->grabMouse();

            if (QWidgetPrivate::keyboardGrabber != 0)
                QWidgetPrivate::keyboardGrabber->grabKeyboard();

        QWidget *fw = QApplicationPrivate::active_window ? QApplicationPrivate::active_window->focusWidget()
              : q_func()->focusWidget();
          if (fw) {
              if (fw != q_func()->focusWidget()) {
                  fw->setFocus(Qt::PopupFocusReason);
              } else {
                  QFocusEvent e(QEvent::FocusIn, Qt::PopupFocusReason);
                  q_func()->sendEvent(fw, &e);
              }
          }
        }
    } else {

        // popups are not focus-handled by the window system (the
        // first popup grabbed the keyboard), so we have to do that
        // manually: A popup was closed, so the previous popup gets
        // the focus.
        QWidget* aw = QApplicationPrivate::popupWidgets->last();
        if (QWidget *fw = QApplication::focusWidget()) {
            QFocusEvent e(QEvent::FocusOut, Qt::PopupFocusReason);
            q_func()->sendEvent(fw, &e);
        }

        // Enable pointer capture for previous popup
        if (aw) {
            aw->effectiveWinId()->SetPointerCapture(true);
        }
    }
}

QWidget * QApplication::topLevelAt(QPoint const& point)
{
    QWidget *found = 0;
    int lowestZ = INT_MAX;
    QWidgetList list = QApplication::topLevelWidgets();
    for (int i = 0; i < list.count(); ++i) {
        QWidget *widget = list.at(i);
        if (widget->isVisible() && !(widget->windowType() == Qt::Desktop)) {
            Q_ASSERT(widget->testAttribute(Qt::WA_WState_Created));
            if (widget->geometry().adjusted(0,0,1,1).contains(point)) {
                // At this point we know there is a Qt widget under the point.
                // Now we need to make sure it is the top most in the z-order.
                RDrawableWindow* rw = widget->d_func()->topData()->rwindow;
                int z = rw->OrdinalPosition();
                if (z < lowestZ) {
                    lowestZ = z;
                    found = widget;
                }
            }
            }
    }
    return found;
}

void QApplication::alert(QWidget * /* widget */, int /* duration */)
{
    // TODO: Implement QApplication::alert(QWidget *widget, int duration)
}

int QApplication::doubleClickInterval()
{
    TTimeIntervalMicroSeconds32 us;
    TInt distance;
    S60->wsSession().GetDoubleClickSettings(us, distance);
    return (us.Int() / 1000);
}

void QApplication::setDoubleClickInterval(int ms)
{
    TTimeIntervalMicroSeconds32 newUs( ms * 1000);
    TTimeIntervalMicroSeconds32 us;
    TInt distance;
    S60->wsSession().GetDoubleClickSettings(us, distance);
    if (us != newUs)
        S60->wsSession().SetDoubleClick(newUs, distance);
}

int QApplication::keyboardInputInterval()
{
    return QApplicationPrivate::keyboard_input_time;
}

void QApplication::setKeyboardInputInterval(int ms)
{
    QApplicationPrivate::keyboard_input_time = ms;
}

int QApplication::cursorFlashTime()
{
    return QApplicationPrivate::cursor_flash_time;
}

void QApplication::setCursorFlashTime(int msecs)
{
    QApplicationPrivate::cursor_flash_time = msecs;
}

void QApplication::beep()
{
    TInt frequency=440;
    TTimeIntervalMicroSeconds duration(500000);
    QS60Beep* beep=NULL;
    TRAPD(err, beep=QS60Beep::NewL(frequency, duration));
    if (!err)
        beep->Play();
    delete beep;
    beep=NULL;
}

/*!
    \warning This function is only available on Symbian.
    \since 4.6

    This function processes an individual Symbian window server
    \a event. It returns 1 if the event was handled, 0 if
    the \a event was not handled, and -1 if the event was
    not handled because the event handle (\c{TWsEvent::Handle()})
    is not known to Qt.
 */
int QApplication::s60ProcessEvent(TWsEvent *event)
{
    bool handled = s60EventFilter(event);
    if (handled)
        return 1;

    // Qt event handling. Handle some events regardless of if the handle is in our
    // widget map or not.
    CCoeControl* control = reinterpret_cast<CCoeControl*>(event->Handle());
    const bool controlInMap = QWidgetPrivate::mapper && QWidgetPrivate::mapper->contains(control);
    switch (event->Type()) {
#if !defined(QT_NO_IM) && defined(Q_WS_S60)
    case EEventKey:
    case EEventKeyUp:
    case EEventKeyDown:
    {
        // The control doesn't seem to be any of our widgets, so rely on the focused
        // widget instead. If the user needs the control, it can be found inside the
        // event structure.
        QWidget *w = qApp ? qApp->focusWidget() : 0;
        if (w) {
            QInputContext *ic = w->inputContext();
            if (ic && ic->s60FilterEvent(w, event)) {
                return 1;
            } else {
                return 0;
            }
        }
        break;
    }
#endif
    case EEventPointerEnter:
        if (controlInMap)
            return 1; // Qt::Enter will be generated in HandlePointerL
        break;
    case EEventPointerExit:
        if (controlInMap) {
            if (S60) {
                // mouseEvent outside our window, send leave event to last focused widget
                QMouseEvent mEvent(QEvent::Leave, S60->lastPointerEventPos, S60->lastCursorPos,
                    Qt::NoButton, QApplicationPrivate::mouse_buttons, Qt::NoModifier);
                if (S60->lastPointerEventTarget)
                    qt_sendSpontaneousEvent(S60->lastPointerEventTarget,&mEvent);
                S60->lastPointerEventTarget = 0;
            }
            return 1;
        }
        break;
    case EEventScreenDeviceChanged:
        if (S60)
            S60->updateScreenSize();
        if (qt_desktopWidget) {
            QSize oldSize = qt_desktopWidget->size();
            qt_desktopWidget->data->crect.setWidth(S60->screenWidthInPixels);
            qt_desktopWidget->data->crect.setHeight(S60->screenHeightInPixels);
            QResizeEvent e(qt_desktopWidget->size(), oldSize);
            QApplication::sendEvent(qt_desktopWidget, &e);
        }
        return 0; // Propagate to CONE
    case EEventWindowVisibilityChanged:
        if (controlInMap) {
            const TWsVisibilityChangedEvent *visChangedEvent = event->VisibilityChanged();
            QWidget *w = QWidgetPrivate::mapper->value(control);
            if (!w->d_func()->maybeTopData())
                break;
            if (visChangedEvent->iFlags & TWsVisibilityChangedEvent::ENotVisible) {
                delete w->d_func()->topData()->backingStore;
                w->d_func()->topData()->backingStore = 0;
            } else if ((visChangedEvent->iFlags & TWsVisibilityChangedEvent::EPartiallyVisible)
                       && !w->d_func()->maybeBackingStore()) {
                w->d_func()->topData()->backingStore = new QWidgetBackingStore(w);
                w->d_func()->invalidateBuffer(w->rect());
                w->repaint();
            }
            return 1;
        }
        break;
    case EEventFocusGained:
#ifndef QT_NO_CURSOR
        //re-enable mouse interaction
        if (S60->mouseInteractionEnabled) {
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
            if (S60->brokenPointerCursors)
                qt_symbian_show_pointer_sprite();
            else
#endif
                S60->wsSession().SetPointerCursorMode(EPointerCursorNormal);
        }
#endif
        break;
    case EEventFocusLost:
#ifndef QT_NO_CURSOR
        //disable mouse as may be moving to application that does not support it
        if (S60->mouseInteractionEnabled) {
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
            if (S60->brokenPointerCursors)
                qt_symbian_hide_pointer_sprite();
            else
#endif
                S60->wsSession().SetPointerCursorMode(EPointerCursorNone);
        }
#endif
        break;
	default:
        break;
    }

    if (!controlInMap)
        return -1;

    return 0;
}

/*!
  \warning This virtual function is only available on Symbian.
  \since 4.6

  If you create an application that inherits QApplication and reimplement
  this function, you get direct access to events that the are received
  from the Symbian window server. The events are passed in the TWsEvent
  \a aEvent parameter.

  Return true if you want to stop the event from being processed. Return
  false for normal event dispatching. The default implementation
  false, and does nothing with \a aEvent.
 */
bool QApplication::s60EventFilter(TWsEvent * /* aEvent */)
{
    return false;
}

/*!
  \warning This function is only available on Symbian.
  \since 4.6

  Handles \a{command}s which are typically handled by
  CAknAppUi::HandleCommandL(). Qts Ui integration into Symbian is
  partially achieved by deriving from CAknAppUi. Currently, exit,
  menu and softkey commands are handled.

  \sa s60EventFilter(), s60ProcessEvent()
*/
void QApplication::symbianHandleCommand(int command)
{
    QScopedLoopLevelCounter counter(d_func()->threadData);
    switch (command) {
#ifdef Q_WS_S60
    case EAknSoftkeyExit: {
        QCloseEvent ev;
        QApplication::sendSpontaneousEvent(this, &ev);
        if (ev.isAccepted())
            quit();
        break;
    }
#endif
    case EEikCmdExit:
        quit();
        break;
    default:
        bool handled = QSoftKeyManager::handleCommand(command);
#ifdef Q_WS_S60
        if (!handled)
            QMenuBarPrivate::symbianCommands(command);
#else
        Q_UNUSED(handled);
#endif
        break;
    }
}

/*!
  \warning This function is only available on Symbian.
  \since 4.6

  Handles the resource change specified by \a type.

  Currently, KEikDynamicLayoutVariantSwitch and
  KAknsMessageSkinChange are handled.
 */
void QApplication::symbianResourceChange(int type)
{
    switch (type) {
#ifdef Q_WS_S60
    case KEikDynamicLayoutVariantSwitch:
        {
        if (S60)
            S60->updateScreenSize();

#ifndef QT_NO_STYLE_S60
        QS60Style *s60Style = 0;

#ifndef QT_NO_STYLE_STYLESHEET
        QStyleSheetStyle *proxy = qobject_cast<QStyleSheetStyle*>(QApplication::style());
        if (proxy)
            s60Style = qobject_cast<QS60Style*>(proxy->baseStyle());
        else
#endif
            s60Style = qobject_cast<QS60Style*>(QApplication::style());

        if (s60Style)
            s60Style->d_func()->handleDynamicLayoutVariantSwitch();
#endif
        }
        break;

#ifndef QT_NO_STYLE_S60
    case KAknsMessageSkinChange:
        if (QS60Style *s60Style = qobject_cast<QS60Style*>(QApplication::style()))
            s60Style->d_func()->handleSkinChange();
        break;
#endif
#endif // Q_WS_S60
    default:
        break;
    }
}

#ifndef QT_NO_WHEELEVENT
int QApplication::wheelScrollLines()
{
    return QApplicationPrivate::wheel_scroll_lines;
}

void QApplication::setWheelScrollLines(int n)
{
    QApplicationPrivate::wheel_scroll_lines = n;
}
#endif //QT_NO_WHEELEVENT

bool QApplication::isEffectEnabled(Qt::UIEffect /* effect */)
{
    // TODO: Implement QApplication::isEffectEnabled(Qt::UIEffect effect)
    return false;
}

void QApplication::setEffectEnabled(Qt::UIEffect /* effect */, bool /* enable */)
{
    // TODO: Implement QApplication::setEffectEnabled(Qt::UIEffect effect, bool enable)
}

TUint QApplicationPrivate::resolveS60ScanCode(TInt scanCode, TUint keysym)
{
    if (keysym) {
        // If keysym is specified, cache it.
        scanCodeCache.insert(scanCode, keysym);
        return keysym;
    } else {
        // If not, retrieve the cached version.
        return scanCodeCache[scanCode];
    }
}

void QApplicationPrivate::initializeMultitouch_sys()
{
#ifdef QT_SYMBIAN_SUPPORTS_ADVANCED_POINTER
    if (HAL::Get(HALData::EPointer3DMaxPressure, maxTouchPressure) != KErrNone)
        maxTouchPressure = KMaxTInt;
#endif
}

void QApplicationPrivate::cleanupMultitouch_sys()
{ }

#ifndef QT_NO_SESSIONMANAGER
QSessionManager::QSessionManager(QApplication * /* app */, QString & /* id */, QString& /* key */)
{

}

QSessionManager::~QSessionManager()
{

}

bool QSessionManager::allowsInteraction()
{
    return false;
}

void QSessionManager::cancel()
{

}
#endif //QT_NO_SESSIONMANAGER

#ifdef QT_KEYPAD_NAVIGATION
/*
 * Show/Hide the mouse cursor depending on phone type and chosen mode
 */
void QApplicationPrivate::setNavigationMode(Qt::NavigationMode mode)
{
#ifndef QT_NO_CURSOR
    const bool wasCursorOn = (QApplicationPrivate::navigationMode == Qt::NavigationModeCursorAuto
        && !S60->hasTouchscreen)
        || QApplicationPrivate::navigationMode == Qt::NavigationModeCursorForceVisible;
    const bool isCursorOn = (mode == Qt::NavigationModeCursorAuto
        && !S60->hasTouchscreen)
        || mode == Qt::NavigationModeCursorForceVisible;

    if (!wasCursorOn && isCursorOn) {
        //Show the cursor, when changing from another mode to cursor mode
        qt_symbian_set_cursor_visible(true);
    }
    else if (wasCursorOn && !isCursorOn) {
        //Hide the cursor, when leaving cursor mode
        qt_symbian_set_cursor_visible(false);
    }
#endif
    QApplicationPrivate::navigationMode = mode;
}
#endif

#ifndef QT_NO_CURSOR
/*****************************************************************************
 QApplication cursor stack
 *****************************************************************************/

void QApplication::setOverrideCursor(const QCursor &cursor)
{
    qApp->d_func()->cursor_list.prepend(cursor);
    qt_symbian_setGlobalCursor(cursor);
}

void QApplication::restoreOverrideCursor()
{
    if (qApp->d_func()->cursor_list.isEmpty())
        return;
    qApp->d_func()->cursor_list.removeFirst();

    if (!qApp->d_func()->cursor_list.isEmpty()) {
        qt_symbian_setGlobalCursor(qApp->d_func()->cursor_list.first());
    }
    else {
        //determine which widget has focus
        QWidget *w = QApplication::widgetAt(QCursor::pos());
#ifndef Q_SYMBIAN_FIXED_POINTER_CURSORS
        if (S60->brokenPointerCursors) {
            qt_symbian_set_pointer_sprite(w ? w->cursor() : Qt::ArrowCursor);
        }
        else
#endif
        {
            //because of the internals of window server, we need to force the cursor
            //to be set in all child windows too, otherwise when the cursor is over
            //the child window it may show a widget cursor or arrow cursor instead,
            //depending on construction order.
            QListIterator<WId> iter(QWidgetPrivate::mapper->uniqueKeys());
            while (iter.hasNext()) {
                CCoeControl *ctrl = iter.next();
                if(ctrl->OwnsWindow()) {
                    ctrl->DrawableWindow()->ClearPointerCursor();
                }
            }
            if (w)
                qt_symbian_setWindowCursor(w->cursor(), w->effectiveWinId());
            else
                qt_symbian_setWindowGroupCursor(Qt::ArrowCursor, S60->windowGroup());
        }
    }
}

#endif // QT_NO_CURSOR

QT_END_NAMESPACE
