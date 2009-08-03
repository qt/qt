#ifndef TAPANDHOLDGESTURE_H
#define TAPANDHOLDGESTURE_H

#include <QtCore/QBasicTimer>
#include <QtGui/QGesture>
#include <QtGui/QWidget>

class TapAndHoldGesture : public QGesture
{
    Q_OBJECT
    Q_PROPERTY(QPoint pos READ pos)

public:
    TapAndHoldGesture(QWidget *parent);

    bool filterEvent(QEvent *event);
    void reset();

    QPoint pos() const;

protected:
    void timerEvent(QTimerEvent *event);

private:
    QBasicTimer timer;
    int iteration;
    QPoint position;
    static const int iterationCount;
    static const int iterationTimeout;
};

#endif // TAPANDHOLDGESTURE_H
