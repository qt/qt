#ifndef QDIRECTFBINPUT_H
#define QDIRECTFBINPUT_H

#include <QSemaphore>
#include <QObject>
#include <QHash>
#include <QPoint>
#include <QEvent>

#include <QtGui/qwindowdefs.h>

#include <directfb.h>

class QDirectFbInput : public QObject
{
    Q_OBJECT
public:
    QDirectFbInput(QObject *parent);
    void addWindow(DFBWindowID id, QWidget *tlw);
    void removeWindow(WId wId);

public slots:
    void runInputEventLoop();
    void stopInputEventLoop();
    void handleEvents();

private:
    void handleMouseEvents(const DFBEvent &event);
    void handleWheelEvent(const DFBEvent &event);
    void handleKeyEvents(const DFBEvent &event);
    void handleEnterLeaveEvents(const DFBEvent &event);
    inline QPoint globalPoint(const DFBEvent &event) const;


    IDirectFB *m_dfbInterface;
    IDirectFBDisplayLayer *m_dfbDisplayLayer;
    IDirectFBEventBuffer *m_eventBuffer;

    bool m_shouldStop;
    QSemaphore m_waitStop;

    QHash<DFBWindowID,QWidget *>m_tlwMap;
};

#endif // QDIRECTFBINPUT_H
