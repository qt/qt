#include "qdirectfbinput.h"
#include "qdirectfbconvenience.h"

#include <QThread>
#include <QDebug>
#include <QWindowSystemInterface>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>

#include <directfb.h>

InputSocketWaiter::InputSocketWaiter(IDirectFBEventBuffer *eventBuffer, QObject *parent)
    : QThread(parent), m_eventBuffer(eventBuffer),m_shouldStop(false)
{
    this->start();
}

InputSocketWaiter::~InputSocketWaiter()
{
    m_shouldStop = true;
    m_eventBuffer->WakeUp(m_eventBuffer);
    m_cleanupMutex.lock();
}

void InputSocketWaiter::continueWaitingForEvents()
{
    m_finishedProcessingEvents.wakeAll();
}

void InputSocketWaiter::run()
{
    m_cleanupMutex.lock();
    while (1) {
        m_eventBuffer->WaitForEvent(m_eventBuffer);
        if (m_shouldStop)
            break;
        emit newEvent();
        QMutex waitForProcessingMutex;
        waitForProcessingMutex.lock();
        m_finishedProcessingEvents.wait(&waitForProcessingMutex);
    }
    m_cleanupMutex.unlock();
}

QDirectFbInput *QDirectFbInput::instance()
{
    static QDirectFbInput *input = 0;
    if (!input) {
        input = new QDirectFbInput();
    }
    return input;
}

QDirectFbInput::QDirectFbInput()
    : QObject()
{
    dfbInterface = QDirectFbConvenience::dfbInterface();

    DFBResult ok = dfbInterface->CreateEventBuffer(dfbInterface,&eventBuffer);
    if (ok != DFB_OK)
        DirectFBError("Failed to initialise eventbuffer", ok);

    dfbInterface->GetDisplayLayer(dfbInterface,DLID_PRIMARY, &dfbDisplayLayer);

    m_inputHandler = new InputSocketWaiter(eventBuffer,this);
    connect(m_inputHandler,SIGNAL(newEvent()),this,SLOT(handleEvents()));

    connect(QApplication::instance(),SIGNAL(aboutToQuit()),SLOT(applicationEnd()));
}

void QDirectFbInput::addWindow(DFBWindowID id, QWidget *tlw)
{
    tlwMap.insert(id,tlw);
    IDirectFBWindow *window;
    dfbDisplayLayer->GetWindow(dfbDisplayLayer,id,&window);

    window->AttachEventBuffer(window,eventBuffer);
}

void QDirectFbInput::removeWindow(QWidget *tlw)
{
    DFBWindowID id = tlwMap.key(tlw,0);
    if (id) {
        IDirectFBWindow *window;
        dfbDisplayLayer->GetWindow(dfbDisplayLayer,id, &window);

        window->DetachEventBuffer(window,eventBuffer);
        tlwMap.remove(id);
    }
}

void QDirectFbInput::handleEvents()
{
    DFBResult hasEvent = eventBuffer->HasEvent(eventBuffer);
    while(hasEvent == DFB_OK){
        DFBEvent event;
        DFBResult ok = eventBuffer->GetEvent(eventBuffer,&event);
        if (ok != DFB_OK)
            DirectFBError("Failed to get event",ok);
        if (event.clazz == DFEC_WINDOW) {
            switch (event.window.type) {
            case DWET_BUTTONDOWN:
            case DWET_BUTTONUP:
            case DWET_MOTION:
                handleMouseEvents(event);
                break;
            case DWET_WHEEL:
                handleWheelEvent(event);
                break;
            case DWET_KEYDOWN:
            case DWET_KEYUP:
                handleKeyEvents(event);
                break;
            case DWET_ENTER:
            case DWET_LEAVE:
                handleEnterLeaveEvents(event);
            default:
                break;
            }

        }

        hasEvent = eventBuffer->HasEvent(eventBuffer);
    }
    m_inputHandler->continueWaitingForEvents();
}

void QDirectFbInput::handleMouseEvents(const DFBEvent &event)
{
    QPoint p(event.window.x, event.window.y);
    QPoint globalPos = globalPoint(event);
    Qt::MouseButtons buttons = QDirectFbConvenience::mouseButtons(event.window.buttons);

    IDirectFBDisplayLayer *layer = QDirectFbConvenience::dfbDisplayLayer();
    IDirectFBWindow *window;
    layer->GetWindow(layer,event.window.window_id,&window);

    long timestamp = (event.window.timestamp.tv_sec*1000) + (event.window.timestamp.tv_usec/1000);

    if (event.window.type == DWET_BUTTONDOWN) {
        window->GrabPointer(window);
    } else if (event.window.type == DWET_BUTTONUP) {
        window->UngrabPointer(window);
    }
    QWidget *tlw = tlwMap.value(event.window.window_id);
    QWindowSystemInterface::handleMouseEvent(tlw, timestamp, p, globalPos, buttons);
}

void QDirectFbInput::applicationEnd()
{
    delete m_inputHandler;
    m_inputHandler = 0;
}

void QDirectFbInput::handleWheelEvent(const DFBEvent &event)
{
    QPoint p(event.window.cx, event.window.cy);
    QPoint globalPos = globalPoint(event);
    long timestamp = (event.window.timestamp.tv_sec*1000) + (event.window.timestamp.tv_usec/1000);
    QWidget *tlw = tlwMap.value(event.window.window_id);
    QWindowSystemInterface::handleWheelEvent(tlw, timestamp, p, globalPos,
                                          event.window.step*120,
                                          Qt::Vertical);
}

void QDirectFbInput::handleKeyEvents(const DFBEvent &event)
{
    QEvent::Type type = QDirectFbConvenience::eventType(event.window.type);
    Qt::Key key = QDirectFbConvenience::keyMap()->value(event.window.key_symbol);
    Qt::KeyboardModifiers modifiers = QDirectFbConvenience::keyboardModifiers(event.window.modifiers);

    long timestamp = (event.window.timestamp.tv_sec*1000) + (event.window.timestamp.tv_usec/1000);

    QChar character;
    if (DFB_KEY_TYPE(event.window.key_symbol) == DIKT_UNICODE)
        character = QChar(event.window.key_symbol);
    QWidget *tlw = tlwMap.value(event.window.window_id);
    QWindowSystemInterface::handleKeyEvent(tlw, timestamp, type, key, modifiers, character);
}

void QDirectFbInput::handleEnterLeaveEvents(const DFBEvent &event)
{
    QWidget *tlw = tlwMap.value(event.window.window_id);
    switch (event.window.type) {
    case DWET_ENTER:
        QWindowSystemInterface::handleEnterEvent(tlw);
        break;
    case DWET_LEAVE:
        QWindowSystemInterface::handleLeaveEvent(tlw);
        break;
    default:
        break;
    }
}

inline QPoint QDirectFbInput::globalPoint(const DFBEvent &event) const
{
    IDirectFBWindow *window;
    dfbDisplayLayer->GetWindow(dfbDisplayLayer,event.window.window_id,&window);
    int x,y;
    window->GetPosition(window,&x,&y);
    return QPoint(event.window.cx +x, event.window.cy + y);
}

