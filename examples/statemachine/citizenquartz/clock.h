#ifndef CLOCK_H
#define CLOCK_H

#include <QGraphicsItem>
#include <QTime>

class ClockButton ;
class ClockDisplay ;
class QStateMachine ;
class QState ;
class QTimerState ;
class QSound ;

class Clock: public QObject, public QGraphicsItem
{
    Q_OBJECT        
public:
    Clock(QGraphicsItem *parent = 0);

    void initializeUi();
    void initializeStateMachine();

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals: 
    void alarm();
    void anyButtonPressed();

public slots:
    void updateTime();
    void playSound();
    void stopSound();

private:
    void initializeDisplaysState(QState *displays);
    void initializeAlarmState(QState *alarmState);
    void initializeRegularState(QState *regular);
    void initializeUpdateState(QState *update);
    void initializeOutState(QState *out);
    void initializeAlarmUpdateState(QState *update);

    QStateMachine *m_stateMachine;
    ClockDisplay *m_clockDisplay;
    ClockButton *m_buttonA;
    ClockButton *m_buttonB;
    ClockButton *m_buttonC;
    ClockButton *m_buttonD;

    QState *m_alarmState;
    QState *m_timeState;
    QState *m_updateState;
    QState *m_regularState;

    QSound *m_alarmSound;
    QTime m_time;
};

#endif // CLOCK_H
