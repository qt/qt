#include "qdirectfbinput.h"
#include "qdirectfbconvenience.h"

#include <QThread>
#include <QDebug>
#include <private/qapplication_p.h>
#include <QMouseEvent>
#include <QEvent>

#include <directfb/directfb.h>

InputSocketWaiter::InputSocketWaiter(IDirectFBEventBuffer *eventBuffer, QObject *parent)
        : QThread(parent), eventBuffer(eventBuffer)
    {
        this->start();
    }

void InputSocketWaiter::run()
{
    while (1) {
        eventBuffer->WaitForEvent(eventBuffer);
        emit newEvent();
    }
}

QDirectFbInput::QDirectFbInput(QObject *parent)
    : QObject(parent)
{
    DFBResult ok = DirectFBCreate(&dfbInterface);
    if (ok != DFB_OK)
        DirectFBError("Failed to initialise QDirectFBInput", ok);

    ok = dfbInterface->CreateEventBuffer(dfbInterface,&eventBuffer);
    if (ok != DFB_OK)
        DirectFBError("Failed to initialise eventbuffer", ok);

    dfbInterface->GetDisplayLayer(dfbInterface,DLID_PRIMARY, &dfbDisplayLayer);

    InputSocketWaiter *inputHandler = new InputSocketWaiter(eventBuffer,this);
    connect(inputHandler,SIGNAL(newEvent()),this,SLOT(handleEvents()));
}

void QDirectFbInput::addWindow(DFBWindowID id, QWidget *tlw)
{
    tlwMap.insert(id,tlw);
    IDirectFBWindow *window;
    dfbDisplayLayer->GetWindow(dfbDisplayLayer,id,&window);

    window->DisableEvents(window,DWET_ALL);
    window->EnableEvents(window,DFBWindowEventType(DWET_ALL));
    window->SetKeySelection(window,DWKS_ALL,NULL,0);
    window->AttachEventBuffer(window,eventBuffer);
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
            case DWET_WHEEL:
                handleMouseEvents(event);
                break;
            case DWET_KEYDOWN:
            case DWET_KEYUP:
                handleKeyEvents(event);
            }

        } else
            qDebug() << "WHAT!";

        hasEvent = eventBuffer->HasEvent(eventBuffer);
    }
}

void QDirectFbInput::handleMouseEvents(const DFBEvent &event)
{
    QEvent::Type type = QDirectFbConvenience::eventType(event.window.type);
    QPoint p(event.window.cx, event.window.cy);
    QPoint globalPos = globalPoint(event);
    Qt::MouseButton button = QDirectFbConvenience::mouseButton(event.window.button);
    Qt::MouseButtons buttons = QDirectFbConvenience::mouseButtons(event.window.buttons);

    QMouseEvent mouseEvent(type,p,globalPos,button, buttons,(Qt::KeyboardModifiers)0);
    QWidget *tlw = tlwMap.value(event.window.window_id);
    QApplicationPrivate::handleMouseEvent(tlw,mouseEvent);
}

void QDirectFbInput::handleKeyEvents(const DFBEvent &event)
{
    QEvent::Type type = QDirectFbConvenience::eventType(event.window.type);
    Qt::Key key = QDirectFbConvenience::keyMap()->value(event.window.key_symbol);

    QKeyEvent keyEvent(type,key,0,QChar(event.window.key_symbol));//,"",true,0);
    QWidget *tlw = tlwMap.value(event.window.window_id);
    QApplicationPrivate::handleKeyEvent(tlw,&keyEvent);
}

inline QPoint QDirectFbInput::globalPoint(const DFBEvent &event) const
{
    IDirectFBWindow *window;
    dfbDisplayLayer->GetWindow(dfbDisplayLayer,event.window.window_id,&window);
    int x,y;
    window->GetPosition(window,&x,&y);
    return QPoint(event.window.cx +x, event.window.cy + y);
}

