#ifndef QDIRECTFBINPUT_H
#define QDIRECTFBINPUT_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QHash>
#include <QPoint>
#include <QEvent>

#include <directfb.h>

class InputSocketWaiter : public QThread
{
    Q_OBJECT
public:
    InputSocketWaiter(IDirectFBEventBuffer *eventBuffer, QObject *parent);
    virtual ~InputSocketWaiter();
    void continueWaitingForEvents();
protected:
    void run();
signals:
    void newEvent();
private:
     IDirectFBEventBuffer *m_eventBuffer;
     bool m_shouldStop;
     QMutex m_cleanupMutex;
     QWaitCondition m_finishedProcessingEvents;
};

class QDirectFbInput : public QObject
{
    Q_OBJECT
public:
    static QDirectFbInput *instance();
    void addWindow(DFBWindowID id, QWidget *tlw);
    void removeWindow(QWidget *tlw);

public slots:
    void handleEvents();
    void applicationEnd();

private:
    QDirectFbInput();

    void handleMouseEvents(const DFBEvent &event);
    void handleWheelEvent(const DFBEvent &event);
    void handleKeyEvents(const DFBEvent &event);
    void handleEnterLeaveEvents(const DFBEvent &event);
    IDirectFB *dfbInterface;
    IDirectFBDisplayLayer *dfbDisplayLayer;
    IDirectFBEventBuffer *eventBuffer;

    QHash<DFBWindowID,QWidget *>tlwMap;

    inline QPoint globalPoint(const DFBEvent &event) const;

    InputSocketWaiter *m_inputHandler;

};

#endif // QDIRECTFBINPUT_H
