#ifndef QDIRECTFBINPUT_H
#define QDIRECTFBINPUT_H

#include <QThread>
#include <QObject>
#include <QHash>
#include <QPoint>
#include <QEvent>

#include <directfb/directfb.h>

class InputSocketWaiter : public QThread
{
    Q_OBJECT
public:
    InputSocketWaiter(IDirectFBEventBuffer *eventBuffer, QObject *parent);
protected:
    void run();
signals:
    void newEvent();
private:
     IDirectFBEventBuffer *eventBuffer;
};

class QDirectFbInput : public QObject
{
    Q_OBJECT
public:
    QDirectFbInput(QObject *parent = 0);

    void addWindow(DFBWindowID id, QWidget *tlw);

public slots:
    void handleEvents();

private:

    void handleMouseEvents(const DFBEvent &event);
    void handleKeyEvents(const DFBEvent &event);
    IDirectFB *dfbInterface;
    IDirectFBDisplayLayer *dfbDisplayLayer;
    IDirectFBEventBuffer *eventBuffer;

    QHash<DFBWindowID,QWidget *>tlwMap;

    inline QPoint globalPoint(const DFBEvent &event) const;

};

#endif // QDIRECTFBINPUT_H
