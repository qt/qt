#include "clock.h"
#include "clockbutton.h"
#include "clockdisplay.h"
#include "propertyaddstate.h"
#include "timeperiod.h"

#include <QStateMachine>
#include <QState>
#include <QHistoryState>

#include <QPainter>
#include <QTimer>
#include <QSound>

Clock::Clock(QGraphicsItem *parent) 
    : QGraphicsItem(parent),
      m_stateMachine(0),
      m_clockDisplay(0),
      m_buttonA(0),
      m_buttonB(0),
      m_buttonC(0),
      m_buttonD(0),
      m_alarmState(0),
      m_timeState(0),
      m_updateState(0),
      m_regularState(0),
      m_displaysHistoryState(0),
      m_alarmSound(new QSound(":/sound/alarm.wav", this))
{
}

void Clock::initializeUi()
{
    QPainterPath path = shape();
    QPointF pap;
    qreal aap;

    m_buttonA = new ClockButton("Button A", this);
    pap = path.pointAtPercent(0.05);
    aap = path.angleAtPercent(0.05);
    m_buttonA->translate(pap.x(), pap.y());
    m_buttonA->rotate(-aap);
    connect(m_buttonA, SIGNAL(pressed()), this, SIGNAL(anyButtonPressed()));

    m_buttonB = new ClockButton("Button B", this);
    pap = path.pointAtPercent(0.77);
    aap = path.angleAtPercent(0.77);
    m_buttonB->translate(pap.x(), pap.y());
    m_buttonB->rotate(-aap);
    connect(m_buttonB, SIGNAL(pressed()), this, SIGNAL(anyButtonPressed()));

    m_buttonC = new ClockButton("Button C", this);
    pap = path.pointAtPercent(0.67);
    aap = path.angleAtPercent(0.67);
    m_buttonC->translate(pap.x(), pap.y());
    m_buttonC->rotate(-aap);
    connect(m_buttonC, SIGNAL(pressed()), this, SIGNAL(anyButtonPressed()));

    m_buttonD = new ClockButton("Button D", this);
    pap = path.pointAtPercent(0.57);
    aap = path.angleAtPercent(0.57);
    m_buttonD->translate(pap.x(), pap.y());
    m_buttonD->rotate(-aap);
    connect(m_buttonD, SIGNAL(pressed()), this, SIGNAL(anyButtonPressed()));

    QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(this);
    label->setText("CITIZEN");    
    label->setPos(0.0 - label->boundingRect().width() / 2.0, -100.0);

    m_clockDisplay = new ClockDisplay(this);
    m_clockDisplay->setCurrentTime(QDateTime::currentDateTime());        

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->setInterval(1);    
    timer->start();
    m_time.start();
}

void Clock::initializeStateMachine()
{
    m_stateMachine = new QStateMachine;

    QState *displays = new QState(m_stateMachine->rootState());
    displays->setObjectName("displays");
    initializeDisplaysState(displays);

    QState *alarmsBeepState = new QState(m_stateMachine->rootState());
    alarmsBeepState->setObjectName("alarmsBeep");
    alarmsBeepState->invokeMethodOnEntry(this, "playSound");
    alarmsBeepState->invokeMethodOnExit(this, "stopSound");

    QTimer *alarmTimeOut = new QTimer(alarmsBeepState);
    alarmTimeOut->setInterval(30000);    
    alarmsBeepState->invokeMethodOnEntry(alarmTimeOut, "start");
    alarmsBeepState->invokeMethodOnExit(alarmTimeOut, "stop");

    displays->addTransition(m_clockDisplay, SIGNAL(alarmTriggered()), alarmsBeepState);
    alarmsBeepState->addTransition(this, SIGNAL(anyButtonPressed()), m_displaysHistoryState); 
    alarmsBeepState->addTransition(alarmTimeOut, SIGNAL(timeout()), m_displaysHistoryState);

    m_stateMachine->setInitialState(displays);
    m_stateMachine->start();
}

void Clock::initializeUpdateState(QState *updateState)
{
    QState *sec = new QState(updateState);
    sec->setObjectName("sec");
    sec->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditSecondMode);
    updateState->setInitialState(sec);
    
    PropertyAddState *secIncrease = new PropertyAddState(updateState);
    secIncrease->setObjectName("sec ++");
    secIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setSeconds(1));
    sec->addTransition(m_buttonD, SIGNAL(pressed()), secIncrease);
    secIncrease->addTransition(sec);

    QState *oneMin = new QState(updateState);
    oneMin->setObjectName("1 min");
    oneMin->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditMinuteMode);
    sec->addTransition(m_buttonC, SIGNAL(pressed()), oneMin);

    PropertyAddState *oneMinIncrease = new PropertyAddState(updateState);
    oneMinIncrease->setObjectName("1 min ++");
    oneMinIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setMinutes(1));
    oneMin->addTransition(m_buttonD, SIGNAL(pressed()), oneMinIncrease);
    oneMinIncrease->addTransition(oneMin);

    QState *tenMin = new QState(updateState);
    tenMin->setObjectName("10 min");
    tenMin->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditMinuteMode);
    oneMin->addTransition(m_buttonC, SIGNAL(pressed()), tenMin);

    PropertyAddState *tenMinIncrease = new PropertyAddState(updateState);
    tenMinIncrease->setObjectName("10 min ++");
    tenMinIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setMinutes(10));
    tenMin->addTransition(m_buttonD, SIGNAL(pressed()), tenMinIncrease);
    tenMinIncrease->addTransition(tenMin);

    QState *hr = new QState(updateState);
    hr->setObjectName("hr");
    hr->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditHourMode);
    tenMin->addTransition(m_buttonC, SIGNAL(pressed()), hr);    

    PropertyAddState *hrIncrease = new PropertyAddState(updateState);
    hrIncrease->setObjectName("hr ++");
    hrIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setHours(1));
    hr->addTransition(m_buttonD, SIGNAL(pressed()), hrIncrease);
    hrIncrease->addTransition(hr);

    QState *mon = new QState(updateState);
    mon->setObjectName("mon");
    mon->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditMonthMode);
    hr->addTransition(m_buttonC, SIGNAL(pressed()), mon);

    PropertyAddState *monIncrease = new PropertyAddState(updateState);
    monIncrease->setObjectName("mon ++");
    monIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setMonths(1));
    mon->addTransition(m_buttonD, SIGNAL(pressed()), monIncrease);
    monIncrease->addTransition(mon);

    QState *day = new QState(updateState);
    day->setObjectName("day");
    day->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditDayMode);
    mon->addTransition(m_buttonC, SIGNAL(pressed()), day);

    PropertyAddState *dayIncrease = new PropertyAddState(updateState);
    dayIncrease->setObjectName("day ++");
    dayIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setDays(1));
    day->addTransition(m_buttonD, SIGNAL(pressed()), dayIncrease);
    dayIncrease->addTransition(day);

    QState *year = new QState(updateState);
    year->setObjectName("year");
    year->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditYearMode);
    day->addTransition(m_buttonC, SIGNAL(pressed()), year);

    PropertyAddState *yearIncrease = new PropertyAddState(updateState);
    yearIncrease->setObjectName("year ++");
    yearIncrease->addToProperty(m_clockDisplay, "currentTime", TimePeriod().setYears(1));
    year->addTransition(m_buttonD, SIGNAL(pressed()), yearIncrease);
    yearIncrease->addTransition(year);
    year->addTransition(m_buttonC, SIGNAL(pressed()), m_timeState);
}

void Clock::initializeRegularState(QState *regular)
{
    m_timeState = new QState(regular);
    m_timeState->setObjectName("time");
    m_timeState->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::CurrentTimeMode);

    QState *date = new QState(regular);
    date->setObjectName("date");
    date->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::CurrentDateMode);
    
    QState *dateTimerState = new QState(date);
    dateTimerState->setObjectName("dateTimerState");
    
    // Date state exits after 2 m
    QTimer *dateTimer = new QTimer(dateTimerState);
    dateTimer->setSingleShot(true);
    dateTimer->setInterval(2*60000);
    dateTimerState->invokeMethodOnEntry(dateTimer, "start");    
    dateTimerState->invokeMethodOnExit(dateTimer, "stop");

    date->setInitialState(dateTimerState);

    m_updateState = new QState(regular);
    m_updateState->setObjectName("update");

    // Update state exits after 2 m
    QTimer *updateTimer = new QTimer(m_updateState);
    updateTimer->setSingleShot(true);
    updateTimer->setInterval(2 * 60000);
    m_updateState->invokeMethodOnEntry(updateTimer, "start");
    m_updateState->invokeMethodOnExit(updateTimer, "stop");

    initializeUpdateState(m_updateState);
    
    m_timeState->addTransition(m_buttonD, SIGNAL(pressed()), date);    
    date->addTransition(m_buttonD, SIGNAL(pressed()), m_timeState);
    date->addTransition(dateTimer, SIGNAL(timeout()), m_timeState);

    m_updateState->addTransition(updateTimer, SIGNAL(timeout()), m_timeState);
    m_updateState->addTransition(m_buttonB, SIGNAL(pressed()), m_timeState);

    regular->setInitialState(m_timeState);
}

void Clock::initializeAlarmUpdateState(QState *update)
{
    QState *hr = new QState(update);
    hr->setObjectName("hr");
    hr->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditAlarmHourMode);

    PropertyAddState *hrInc = new PropertyAddState(update);
    hrInc->setObjectName("hr ++");
    hrInc->addToProperty(m_clockDisplay, "alarm", TimePeriod().setHours(1));
    hr->addTransition(m_buttonD, SIGNAL(pressed()), hrInc);
    hrInc->addTransition(hr);

    QState *tenMin = new QState(update);
    tenMin->setObjectName("10 min");
    tenMin->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditAlarmTenMinuteMode);
    hr->addTransition(m_buttonC, SIGNAL(pressed()), tenMin);

    PropertyAddState *tenMinInc = new PropertyAddState(update);
    tenMinInc->setObjectName("10 min ++");
    tenMinInc->addToProperty(m_clockDisplay, "alarm", TimePeriod().setMinutes(10));
    tenMin->addTransition(m_buttonD, SIGNAL(pressed()), tenMinInc);
    tenMinInc->addTransition(tenMin);

    QState *oneMin = new QState(update);
    oneMin->setObjectName("1 min");
    oneMin->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::EditAlarmMinuteMode);
    tenMin->addTransition(m_buttonC, SIGNAL(pressed()), oneMin);

    PropertyAddState *oneMinInc = new PropertyAddState(update);
    oneMinInc->setObjectName("1 min ++");
    oneMinInc->addToProperty(m_clockDisplay, "alarm", TimePeriod().setMinutes(1));
    oneMin->addTransition(m_buttonD, SIGNAL(pressed()), oneMinInc);
    oneMinInc->addTransition(oneMin);
    
    oneMin->addTransition(m_buttonC, SIGNAL(pressed()), m_alarmState);    
    m_alarmState->addTransition(m_buttonC, SIGNAL(pressed()), hr);    
}

void Clock::initializeOutState(QState *out)
{
    m_alarmState = new QState(out);
    m_alarmState->setObjectName("alarmState");
    m_alarmState->setPropertyOnEntry(m_clockDisplay, "displayMode", ClockDisplay::AlarmMode);
    initializeAlarmState(m_alarmState);
    out->setInitialState(m_alarmState);

    QState *alarmUpdate = new QState(out);
    alarmUpdate->setObjectName("alarmUpdate");
    initializeAlarmUpdateState(alarmUpdate);

    alarmUpdate->addTransition(m_buttonB, SIGNAL(pressed()), m_alarmState);
    m_alarmState->addTransition(m_buttonA, SIGNAL(pressed()), m_regularState);
}

void Clock::initializeDisplaysState(QState *displays)
{
    m_regularState = new QState(displays);
    m_regularState->setObjectName("regular");
    initializeRegularState(m_regularState);
    displays->setInitialState(m_regularState);

    QState *out = new QState(displays);
    out->setObjectName("out");

    QTimer *outTimer = new QTimer(out);
    outTimer->setSingleShot(true);
    outTimer->setInterval(2 * 60000);
    out->invokeMethodOnEntry(outTimer, "start");
    out->invokeMethodOnExit(outTimer, "stop");    

    initializeOutState(out);
        
    QState *wait = new QState(displays);
    wait->setObjectName("wait");

    QTimer *waitTimer = new QTimer(wait);
    waitTimer->setSingleShot(true);
    waitTimer->setInterval(2000);
    wait->invokeMethodOnEntry(waitTimer, "start");
    wait->invokeMethodOnExit(waitTimer, "stop");

    m_displaysHistoryState = displays->addHistoryState(QState::DeepHistory);

    m_timeState->addTransition(m_buttonC, SIGNAL(pressed()), wait);
    wait->addTransition(waitTimer, SIGNAL(timeout()), m_updateState);
    wait->addTransition(m_buttonC, SIGNAL(released()), m_timeState);
    m_timeState->addTransition(m_buttonA, SIGNAL(pressed()), m_alarmState);
    out->addTransition(outTimer, SIGNAL(timeout()), m_regularState);
}

void Clock::initializeAlarmState(QState *alarmState)
{
    QState *offState = new QState(alarmState);
    offState->setObjectName("alarmOff");
    offState->setPropertyOnEntry(m_clockDisplay, "alarmEnabled", false);
    
    QState *onState = new QState(alarmState);
    onState->setObjectName("alarmOn");
    onState->setPropertyOnEntry(m_clockDisplay, "alarmEnabled", true);

    QHistoryState *history = alarmState->addHistoryState();    
    history->setObjectName("alarmHistory");
    history->setDefaultState(offState);

    offState->addTransition(m_buttonD, SIGNAL(pressed()), onState);
    onState->addTransition(m_buttonD, SIGNAL(pressed()), offState);

    QState *intermediate = new QState(alarmState);
    intermediate->addTransition(history);

    alarmState->setInitialState(intermediate);    
}

QRectF Clock::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath Clock::shape() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(-140.0, -100.0, 280.0, 200.0), 50.0, 50.0, Qt::RelativeSize);

    return path;
}

void Clock::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) 
{
    painter->setPen(Qt::black);
    painter->setBrush(Qt::NoBrush);

    // Clock face
    painter->drawPath(shape());    
}

void Clock::updateTime()
{
    int elapsed = m_time.elapsed();
    if (elapsed > 0) {
        m_time.restart();
        QDateTime currentTime = m_clockDisplay->currentTime();
        m_clockDisplay->setCurrentTime(currentTime.addMSecs(elapsed));

        update();
    }
}

void Clock::playSound()
{
    qDebug("playing sound");
    m_alarmSound->stop();
    m_alarmSound->play();
}

void Clock::stopSound()
{
    qDebug("stopping sound");
    m_alarmSound->stop();
}

