#include "qdirectfbinput.h"
#include "qdirectfbconvenience.h"

#include <QThread>
#include <QDebug>
#include <QWindowSystemInterface>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>

#include <directfb.h>

QDirectFbInput::QDirectFbInput(QObject *parent)
    : QObject(parent), m_shouldStop(false)
{
    m_dfbInterface = QDirectFbConvenience::dfbInterface();

    DFBResult ok = m_dfbInterface->CreateEventBuffer(m_dfbInterface,&m_eventBuffer);
    if (ok != DFB_OK)
        DirectFBError("Failed to initialise eventbuffer", ok);

    m_dfbInterface->GetDisplayLayer(m_dfbInterface,DLID_PRIMARY, &m_dfbDisplayLayer);

}

void QDirectFbInput::runInputEventLoop()
{
    while (true) {
        m_eventBuffer->WaitForEvent(m_eventBuffer);
        if (m_shouldStop) {
            m_waitStop.release();
            break;
        }
        handleEvents();
    }
}

void QDirectFbInput::stopInputEventLoop()
{
    m_shouldStop = true;
    m_waitStop.acquire();
}

void QDirectFbInput::addWindow(DFBWindowID id, QWidget *tlw)
{
    m_tlwMap.insert(id,tlw);
    IDirectFBWindow *window;
    m_dfbDisplayLayer->GetWindow(m_dfbDisplayLayer,id,&window);

    window->AttachEventBuffer(window,m_eventBuffer);
}

void QDirectFbInput::removeWindow(WId wId)
{
    IDirectFBWindow *window;
    m_dfbDisplayLayer->GetWindow(m_dfbDisplayLayer,wId, &window);

    window->DetachEventBuffer(window,m_eventBuffer);
    m_tlwMap.remove(wId);
}

void QDirectFbInput::handleEvents()
{
    DFBResult hasEvent = m_eventBuffer->HasEvent(m_eventBuffer);
    while(hasEvent == DFB_OK){
        DFBEvent event;
        DFBResult ok = m_eventBuffer->GetEvent(m_eventBuffer,&event);
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

        hasEvent = m_eventBuffer->HasEvent(m_eventBuffer);
    }
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
    QWidget *tlw = m_tlwMap.value(event.window.window_id);
    QWindowSystemInterface::handleMouseEvent(tlw, timestamp, p, globalPos, buttons);
}

void QDirectFbInput::handleWheelEvent(const DFBEvent &event)
{
    QPoint p(event.window.cx, event.window.cy);
    QPoint globalPos = globalPoint(event);
    long timestamp = (event.window.timestamp.tv_sec*1000) + (event.window.timestamp.tv_usec/1000);
    QWidget *tlw = m_tlwMap.value(event.window.window_id);
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
    QWidget *tlw = m_tlwMap.value(event.window.window_id);
    QWindowSystemInterface::handleKeyEvent(tlw, timestamp, type, key, modifiers, character);
}

void QDirectFbInput::handleEnterLeaveEvents(const DFBEvent &event)
{
    QWidget *tlw = m_tlwMap.value(event.window.window_id);
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
    m_dfbDisplayLayer->GetWindow(m_dfbDisplayLayer,event.window.window_id,&window);
    int x,y;
    window->GetPosition(window,&x,&y);
    return QPoint(event.window.cx +x, event.window.cy + y);
}

