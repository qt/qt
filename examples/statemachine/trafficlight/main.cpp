/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstate.h>
#include <qstatemachine.h>
#include <qfinalstate.h>
#endif

//! [0]
class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = 0)
        : QWidget(parent), m_color(color), m_on(false) {}

    bool isOn() const
        { return m_on; }
    void setOn(bool on)
    {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }

protected:
    virtual void paintEvent(QPaintEvent *)
    {
        if (!m_on)
            return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(m_color);
        painter.drawEllipse(0, 0, width(), height());
    }

private:
    QColor m_color;
    bool m_on;
};
//! [0]

//! [1]
class LightState : public QState
{
public:
    LightState(LightWidget *light, int duration, QState *parent = 0)
        : QState(parent)
    {
        QTimer *timer = new QTimer(this);
        timer->setInterval(duration);
        timer->setSingleShot(true);
        QState *timing = new QState(this);
        timing->setPropertyOnEntry(light, "on", true);
        timing->invokeMethodOnEntry(timer, "start");
        timing->setPropertyOnExit(light, "on", false);
        QFinalState *done = new QFinalState(this);
        timing->addTransition(timer, SIGNAL(timeout()), done);
        setInitialState(timing);
    }
};
//! [1]

//! [2]
class TrafficLightWidget : public QWidget
{
public:
    TrafficLightWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        m_red = new LightWidget(Qt::red);
        vbox->addWidget(m_red);
        m_yellow = new LightWidget(Qt::yellow);
        vbox->addWidget(m_yellow);
        m_green = new LightWidget(Qt::green);
        vbox->addWidget(m_green);
        QPalette pal = palette();
        pal.setColor(QPalette::Background, Qt::black);
        setPalette(pal);
        setAutoFillBackground(true);
    }

    LightWidget *redLight() const
        { return m_red; }
    LightWidget *yellowLight() const
        { return m_yellow; }
    LightWidget *greenLight() const
        { return m_green; }

private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
};
//! [2]

//! [3]
class TrafficLight : public QWidget
{
public:
    TrafficLight(QWidget *parent = 0)
        : QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        TrafficLightWidget *widget = new TrafficLightWidget();
        vbox->addWidget(widget);

        QStateMachine *machine = new QStateMachine(this);
        LightState *redGoingYellow = new LightState(widget->redLight(), 3000);
        redGoingYellow->setObjectName("redGoingYellow");
        LightState *yellowGoingGreen = new LightState(widget->yellowLight(), 1000);
        yellowGoingGreen->setObjectName("yellowGoingGreen");
        redGoingYellow->addFinishedTransition(yellowGoingGreen);
        LightState *greenGoingYellow = new LightState(widget->greenLight(), 3000);
        greenGoingYellow->setObjectName("greenGoingYellow");
        yellowGoingGreen->addFinishedTransition(greenGoingYellow);
        LightState *yellowGoingRed = new LightState(widget->yellowLight(), 1000);
        yellowGoingRed->setObjectName("yellowGoingRed");
        greenGoingYellow->addFinishedTransition(yellowGoingRed);
        yellowGoingRed->addFinishedTransition(redGoingYellow);

        machine->addState(redGoingYellow);
        machine->addState(yellowGoingGreen);
        machine->addState(greenGoingYellow);
        machine->addState(yellowGoingRed);
        machine->setInitialState(redGoingYellow);
        machine->start();
    }
};
//! [3]

//! [4]
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TrafficLight widget;
    widget.resize(120, 300);
    widget.show();

    return app.exec();
}
//! [4]

#include "main.moc"
