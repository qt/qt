#include "touchwidget.h"

#include <QApplication>
#include <QtEvents>
#include <QTimer>
#include <QTouchEvent>

void TouchWidget::reset()
{
    acceptTouchBegin
        = acceptTouchUpdate
        = acceptTouchEnd
        = seenTouchBegin
        = seenTouchUpdate
        = seenTouchEnd
        = closeWindowOnTouchEnd

        = acceptMousePress
        = acceptMouseMove
        = acceptMouseRelease
        = seenMousePress
        = seenMouseMove
        = seenMouseRelease
        = closeWindowOnMouseRelease

        = false;
}

bool TouchWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
        seenTouchBegin = true;
        if (acceptTouchBegin) {
            event->accept();
            return true;
        }
        break;
    case QEvent::TouchUpdate:
        seenTouchUpdate = true;
        if (acceptTouchUpdate) {
            event->accept();
            return true;
        }
        break;
    case QEvent::TouchEnd:
        seenTouchEnd = true;
        if (closeWindowOnTouchEnd)
            window()->close();
        if (acceptTouchEnd) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
        seenMousePress = true;
        if (acceptMousePress) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseMove:
        seenMouseMove = true;
        if (acceptMouseMove) {
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        seenMouseRelease = true;
        if (closeWindowOnMouseRelease)
            window()->close();        
        if (acceptMouseRelease) {
            event->accept();
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
}
